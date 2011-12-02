<?php

/**
 * Handler for filtering by geometry distance.
 */
class geoquery_handler_relationship_geometry_distance extends views_handler_relationship {
  function option_definition() {
    $options = parent::option_definition();
    $options['distance'] = array('default' => 0.02);
    $options['geometry_field'] = array('default' => 'geom');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['distance'] = array(
      '#type' => 'textfield',
      '#title' => t('Distance'),
      '#default_value' => $this->options['distance'],
      '#description' => t('The buffer value to measure distance of nearby features.'),
    );
    $form['geometry_field'] = array(
      '#type' => 'textfield',
      '#title' => t('Geometry field'),
      '#default_value' => $this->options['geometry_field'],
      '#description' => t('The geometry field in the database.'),
    );
  }

  function query($group_by = FALSE) {
    $join = new views_join_subquery_condition_distance_buffer;
    $join->distance = $this->options['distance'];
    $join->construct($this->definition['base'], $this->definition['base'] . '_nearby', $this->options['geometry_field'], $this->options['geometry_field']);
    $this->alias = $this->query->add_relationship($this->definition['base'] . '_nearby', $join, $this->definition['base']);
  }
}

/**
 * Join handler for relationships that join with a subquery.
 * eg:
 *  LEFT JOIN node node_term_data ON ([YOUR SUBQUERY HERE])
 *
 * join definition
 *   same as views_join class above, except:
 *   - left_query: The subquery to use in the left side of the join clause.
 */
class views_join_subquery_condition_distance_buffer extends views_join_subquery {

  /**
   * Build the SQL for the join this object represents.
   *
   * @param $select_query
   *   An implementation of SelectQueryInterface.
   * @param $table
   *   The base table to join.
   * @param $view_query
   *   The source query, implementation of views_plugin_query.
   * @return
   *
   */
  function build_join($select_query, $table, $view_query) {
    if (empty($this->definition['table formula'])) {
      $right_table = "{" . $this->table . "}";
    }
    else {
      $right_table = $this->definition['table formula'];
    }

    // This is why we subclass views_join_subquery, we are using unusual join conditions.
    $condition = "ST_DWithin($this->table.$this->field, $this->left_table.$this->left_field, $this->distance)";
    $arguments = array();

    // Tack on the extra.
    // This is just copied verbatim from the parent class, which itself has a bug: http://drupal.org/node/1118100
    if (isset($this->extra)) {
      if (is_array($this->extra)) {
        $extras = array();
        foreach ($this->extra as $info) {
          $extra = '';
          // Figure out the table name. Remember, only use aliases provided
          // if at all possible.
          $join_table = '';
          if (!array_key_exists('table', $info)) {
            $join_table = $table['alias'] . '.';
          }
          elseif (isset($info['table'])) {
            $join_table = $info['table'] . '.';
          }

          $placeholder = ':views_join_condition_' . $select_query->nextPlaceholder();

          if (is_array($info['value'])) {
            $operator = !empty($info['operator']) ? $info['operator'] : 'IN';
            // Transform from IN() notation to = notation if just one value.
            if (count($info['value']) == 1) {
              $info['value'] = array_shift($info['value']);
              $operator = $operator == 'NOT IN' ? '!=' : '=';
            }
          }
          else {
            $operator = !empty($info['operator']) ? $info['operator'] : '=';
          }

          $extras[] = "$join_table$info[field] $operator $placeholder";
          $arguments[$placeholder] = $info['value'];
        }

        if ($extras) {
          if (count($extras) == 1) {
            $condition .= ' AND ' . array_shift($extras);
          }
          else {
            $condition .= ' AND (' . implode(' ' . $this->extra_type . ' ', $extras) . ')';
          }
        }
      }
      elseif ($this->extra && is_string($this->extra)) {
        $condition .= " AND ($this->extra)";
      }
    }

    $select_query->addJoin($this->type, $right_table, $table['alias'], $condition, $arguments);
  }
}
