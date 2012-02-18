<?php

/**
 * @file
 * Argument handler to filter results by geometry.
 */

/**
 * Argument handler to filter results by whether their geometries fall within a
 * bounding box.
 */
class geoquery_views_handler_argument_geometry extends views_handler_argument {

  /**
   * Provide defaults for the argument when a new one is created.
   */
  function options(&$options) {
    parent::options($options);
  }

  /**
   * Set up the query for this argument.
   *
   * The argument sent may be found at $this->argument.
   */
  function query() {
    $this->ensure_my_table();
    $placeholder = $this->placeholder();
    $expr = "ST_Intersects(ST_Transform($this->table_alias.$this->real_field, 900913), ";
    $expr .= "ST_SetSrid(ST_GeomFromText($placeholder), 900913))";
    $this->query->add_where_expression(0, $expr, array($placeholder => $this->argument));
  }
}
