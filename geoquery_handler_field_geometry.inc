<?php

/**
 * Field handler that outputs WKT from PostGIS.
 */
class geoquery_handler_field_geometry extends views_handler_field {
  function query() {
    $this->ensure_my_table();
    // Add the field.
    $params = $this->options['group_type'] != 'group' ? array('function' => $this->options['group_type']) : array();
    $this->field_alias = $this->query->add_field(NULL, 'ST_AsText(' . $this->table_alias . '.' . $this->real_field . ')', $this->table_alias . '_' . $this->real_field, $params);

    $this->add_additional_fields();
  }
}