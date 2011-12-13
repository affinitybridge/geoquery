This is for Drupal 7

Views handlers for running geospatial queries against a PostGIS database.

This module is dependent on the Sync PostGIS module which allows you to
synchronise all geofield data to a PostGIS database - Drupal / MySQL remains the
"Database of Record", but data is replicated into postGIS.

sync_postgis:   https://github.com/affinitybridge/sync_postgis

This module provides Views handlers so that you can build views against
your PostGIS tables, including relationship handlers for performing
distance buffer and intersection queries.

Installation
------------

1. Follow the instructions in the Sync PostGIS module to make sure you have
your PostGIS db set up correctly.

2. Install the module as normal.

Use - Views handlers
---------------------
Once the module has been installed, you will now have an extra option when you
go to add a view. In the dropdown box beside the word "Show" where you choose
the base table of the view (where the default option, "Content", equates to the
node table), one of the options will be "PostGIS content". If you choose this
option you will have two special relationships available to you for building
geospatial queries.

Use - One-off spatial queries
------------------------------
The module also provides some functions for running spatial queries on the nodes
in the pg_node table. For example, you can call
geoquery_intersects($nid1, $nid2)
passing in two node ids that exist in the pg_node table, and it will return a
boolean indiciating whether the geometries of the two nodes intersect.

Credits
------------
The code for the all the Views handlers was originally written by Tylor Sherman
(github.com/tylor) and adapted by Katherine Bailey (github.com/katbailey) to be
more generically applicable.
