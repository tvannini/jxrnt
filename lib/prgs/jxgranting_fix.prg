<?php
//2.2
//o2def::module("");
//o2def::folder("");
/*
o2def::prgnotes("This program fixes data in granting tables.

Blank named items are removed from users, roles, rights, roles_per_user, rights_per_role.

Records referencing to unexisting items are removed from tables roles_per_user and rights_per_role.");
*/

o2def::prg("jxgranting_fix", "start", "", __FILE__);

o2def::view("users", False, "", "", 0);
o2def::view("roles", False, "", "", 0);
o2def::view("rights", False, "", "", 0);
o2def::view("rights_per_role", False, "", "", 0);
o2def::view("roles_per_user", False, "", "", 0);
o2def::view("rights_per_role_2", False, "", "", 0);
o2def::view("roles_per_user_2", False, "", "", 0);
o2def::view("rights_per_role_dep", False, "", "", 0);
o2def::view("rights_per_role_dep_2", False, "", "", 0);
o2def::view("roles_per_user_dep", False, "", "", 0);
o2def::view("roles_per_user_dep_2", False, "", "", 0);
o2def::view("descriptions", False, "", "", 0);
o2def::view("role_desc_dep", False, "", "", 0);
o2def::view("right_desc_dep", False, "", "", 0);
o2def::act("start", "");
?>
