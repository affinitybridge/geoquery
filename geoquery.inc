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
   * This provides us with the functionality of "overloaded" methods, which we
   * need for our booleanRelQuery.
   */
  public function __call($method, $parameters) {
    foreach (get_class_methods(__CLASS__) as $class_method) {
      // Our method naming convention is [base_method_name]__[specific_version],
      // e.g. booleanRelQuery__nids which is the version of the booleanRelQuery
      // method that takes two nids as its second and third parameters.
      if (preg_match('/' . $method . '__[a-z]+/', $class_method)) {
        $reflect_method = new ReflectionMethod(__CLASS__, $class_method);
        if (count($parameters) == $reflect_method->getNumberOfParameters()) {
          $class_parameters = $reflect_method->getParameters();
          for ($p = 0; $p < count($parameters); $p++) {
            $match = FALSE;
            if ($class_parameters[$p]->getClass()) {
              $match = get_class($parameters[$p]) == $class_parameters[$p]->getClass()->getName();
            }
            elseif (is_array($parameters[$p]) || $class_parameters[$p]->isArray()) {
              $match = is_array($parameters[$p]) && $class_parameters[$p]->isArray();
            }
            else {
              // Accept anything as we can't do any further type checking for this
              // particular parameter :-(
              $match = TRUE;
            }
            if ($match) {
              if ($p == count($parameters) - 1) {
                $ret = call_user_func_array(array(&$this, $class_method), $parameters);
                break 2; // Method Signature Match
              }
            }
            else {
              break;
            }
          }
        }
      }
    }
    if ($ret) {
      return $ret;
    }
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
   * @param int $nid2
   *  The node id of the second node.
   *
   * @return boolean
   *  The result of the relationship query.
   *
   */
  public function booleanRelQuery__nids($type, $nid1, $nid2, $distance = NULL) {
    // Get the main part of the SELECT statement.
    $sql = $this->prepareBooleanRelQuery($type, $distance);
    // Add on our WHERE clause.
    $sql .= ' WHERE a.nid = :nid1 AND b.nid = :nid2';
    $result = $this->query($sql, array(':nid1' => $nid1, ':nid2' => $nid2));
    return $this->evaluateBooleanRelQuery($result);
  }

  /**
   * Performs a geometry relationship query on the geometries defined by arrays
   * of ids in the postgis pg_node table. This is what allows to perform geo-
   * spatial queries on a node's data during presave, i.e. before it has a nid.
   *
   * @param string $type
   *  The type of query to run, e.g. 'ST_Intersects'.
   *
   * @param array $item1
   *  An array of ids corresponding to rows in the postgis pg_node table.
   *
   * @param array $item2
   *  An array of ids corresponding to rows in the postgis pg_node table.
   *
   * @return boolean
   *  The result of the relationship query.
   *
   */
  public function booleanRelQuery__arrays($type, array $item1, array $item2, $distance = NULL) {
    // Get the main part of the SELECT statement.
    $sql = $this->prepareBooleanRelQuery($type, $distance);
    // Add on our WHERE clause.
    $sql .= ' WHERE a.id IN (' . implode(',', $item1) . ')';
    $sql .= ' AND b.id IN (' . implode(',', $item1) . ')';

    $result = $this->query($sql);
    return $this->evaluateBooleanRelQuery($result);
  }

  /**
   * Performs a geometry relationship query on two sets of geometries, the first
   * defined by a node id and the second defined by an array of ids in the
   * postgis pg_node table.
   *
   * @param string $type
   *  The type of query to run, e.g. 'ST_Intersects'.
   *
   * @param int $nid
   *  The node id of the first node.
   *
   * @param array $arr
   *  An array of ids corresponding to rows in the postgis pg_node table - these
   *  are for the second node, for which we don't have a nid.
   *
   * @return boolean
   *  The result of the relationship query.
   *
   */
  public function booleanRelQuery__nidfirst($type, $nid, array $arr, $distance = NULL) {
    // Get the main part of the SELECT statement.
    $sql = $this->prepareBooleanRelQuery($type, $distance);
    // Add on our WHERE clause.
    $sql .= ' WHERE a.nid = :nid';
    $sql .= ' AND b.id IN (' . implode(',', $arr) . ')';

    $result = $this->query($sql, array(':nid' => $nid));
    return $this->evaluateBooleanRelQuery($result);
  }

  /**
   * Performs a geometry relationship query on two sets of geometries, the first
   * defined by an array of ids in the postgis pg_node table and the second
   * defined by a node id.
   *
   * @param string $type
   *  The type of query to run, e.g. 'ST_Intersects'.
   *
   * @param int $arr
   *  An array of ids corresponding to rows in the postgis pg_node table - these
   *  are for the second node, for which we don't have a nid.
   *
   * @param int $nid
   *  The node id of the first node.
   *
   * @return boolean
   *  The result of the relationship query.
   *
   */
  public function booleanRelQuery__arrfirst($type, array $arr, $nid, $distance = NULL) {
    // Get the main part of the SELECT statement.
    $sql = $this->prepareBooleanRelQuery($type, $distance);
    // Add on our WHERE clause.

    $sql .= ' WHERE a.id IN (' . implode(',', $arr) . ')';
    $sql .= ' AND b.nid = :nid';
    dpm($sql);
    $result = $this->query($sql, array(':nid' => $nid));
    return $this->evaluateBooleanRelQuery($result);
  }

  /**
   * Builds the select statement for a geometry relationship query without a
   * WHERE clause.
   */
  protected function prepareBooleanRelQuery($type, $distance) {
    $table = $this->geo_node_table;
    $geo_column = $this->geometry_column;
    $sql = "SELECT $type(a." . $geo_column . ", b." . $geo_column;
    // Certain geometry relationship queries, such as ST_DWithin, have a third
    // parameter, which is the buffer distance to check.
    if (is_numeric($distance)) {
      $sql .= ", " . $distance;
    }
    $sql .= ") AS relation FROM {" . $table . "} AS a CROSS JOIN {" . $table . "} AS b";
    return $sql;
  }

  /**
   * Given a result set from a boolean rel query, returns true if one of the rows
   * has TRUE as its relation value.
   */
  protected function evaluateBooleanRelQuery(DatabaseStatementInterface $result) {
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
