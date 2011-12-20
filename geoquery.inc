<?php

class geoQuery {

  private $db_key;

  private $geometry_column;

  private $geo_node_table;

  public function __construct($column = 'geom', $table = 'pg_node', $key = SYNC_POSTGIS_DB) {
    $this->geometry_column = $column;
    $this->geo_node_table = $table;
    $this->db_key = $key;
  }

  /**
   * Performs a query on the PostGIS database.
   *
   * @param string $sql
   *   The query to run.
   *
   * @param array $args
   *   An array of arguments to replace the placeholders in the query.
   *
   * @param array $options
   *   An array of options to pass to the query.
   *
   * @return DatabaseStatementInterface
   *   A prepared statement object, already executed.
   */
  public function query($sql, $args = array(), $options = array()) {
    try {
      $result = Database::getConnection('default', $this->db_key)
        ->query($sql, $args, $options);
      return $result;
    }
    catch (Exception $e) {
      watchdog_exception('geoquery', $e);
      return NULL;
    }
  }

  /**
   * Performs a geometry relationship query on the geometries of two nodes.
   *
   * @param string $type
   *  The type of query to run, e.g. 'ST_Intersects'.
   *
   * @param int $nid1
   *  The node id of the first node.
   *
   * @param $nid2
   *  The node id of the second node.
   *
   * @return boolean
   *  The result of the relationship query.
   * 
   */
  public function booleanRelQuery($type, $nid1, $nid2, $distance = NULL) {
    $table = $this->geo_node_table;
    $geo_column = $this->geometry_column;
    $sql = "SELECT $type(a." . $geo_column . ", b." . $geo_column;
    // Certain geometry relationship queries, such as ST_DWithin, have a third
    // parameter, which is the buffer distance to check.
    if (is_numeric($distance)) {
      $sql .= ", " . $distance;
    }
    $sql .= ") AS relation FROM {" . $table . "} AS a CROSS JOIN {" . $table . "} AS b
            WHERE a.nid = :nid1 AND b.nid = :nid2";
    $result = $this->query($sql, array(':nid1' => $nid1, ':nid2' => $nid2));

    if (!$result) {
      return FALSE;
    }

    foreach ($result as $row) {
      if ($row->relation == TRUE) {
        return TRUE;
      }
    }
    return FALSE;
  }
}
