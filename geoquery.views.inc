<?php

function geoquery_views_data() {
  $data = array();

  /**
   * Node table
   */
  $data['pg_node']['table']['group'] = t('PostGIS content');
  $data['pg_node']['table']['base'] = array(
    'field' => 'id',
    'title' => t('PostGIS content'),
    'help' => t("Geo information in the PostGIS database."),
    'weight' => -10,
    'database' => SYNC_POSTGIS_DB,
  );

  $data['pg_node']['id'] = array(
    'title' => t('ID'),
    'help' => t('Primary PostGIS node identifier.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'label' => t('Nearby geometries'),
      'handler' => 'geoquery_handler_relationship_geometry_distance',
      'base' => 'pg_node',
      'field' => 'id',
    ),
  );

  // We create an extra handler definition on the id field which has the
  // "intersection" relationship handler
  $data['pg_node']['id_intersect'] = array(
    'real field' => 'id',
    'title' => t('Intersect'),
    'help' => t('Add an intersection relationship.'),
    'relationship' => array(
      'label' => t('Intersecting geometries'),
      'handler' => 'geoquery_handler_relationship_geometry_intersection',
      'base' => 'pg_node',
      'field' => 'id',
    ),
  );

  $data['pg_node']['nid'] = array(
    'title' => t('Node id'),
    'help' => t('Corresponds to the node id in the drupal db.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['pg_node']['title'] = array(
    'title' => t('PG node title'),
    'help' => t('The node title as stored in PG.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['pg_node']['type'] = array(
    'title' => t('PG node type'),
    'help' => t('The node type as stored in PG.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['pg_node']['field'] = array(
    'title' => t('Geo field'),
    'help' => t('The geofield this data was syncd from.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['pg_node']['delta'] = array(
    'title' => t('Geo field delta'),
    'help' => t('The field delta this data corresponds to.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['pg_node']['geom'] = array(
    'title' => t('Node geometry'),
    'help' => t('Display node location in WKT.'),
    'field' => array(
      'handler' => 'geoquery_handler_field_geometry',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'geoquery_views_handler_argument_geometry',
    ),
  );

  $data['pg_node']['geom_albers'] = array(
    'title' => t('Node albers geometry'),
    'help' => t('Display node location in albers format.'),
    'field' => array(
      'handler' => 'geoquery_handler_field_geometry',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}