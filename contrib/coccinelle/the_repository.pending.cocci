// This file is used for the ongoing refactoring of
// bringing the index or repository struct in all of
// our code base.

@@
expression E;
expression F;
expression G;
@@
- read_object_file(
+ repo_read_object_file(the_repository,
  E, F, G)

@@
expression E;
@@
- has_object_file(
+ repo_has_object_file(the_repository,
  E)

@@
expression E;
expression F;
@@
- has_object_file_with_flags(
+ repo_has_object_file_with_flags(the_repository,
  E)

@@
expression E;
expression F;
expression G;
@@
- parse_cummit_internal(
+ repo_parse_cummit_internal(the_repository,
  E, F, G)

@@
expression E;
expression F;
@@
- parse_cummit_gently(
+ repo_parse_cummit_gently(the_repository,
  E, F)

@@
expression E;
@@
- parse_cummit(
+ repo_parse_cummit(the_repository,
  E)

@@
expression E;
expression F;
@@
- get_merge_bases(
+ repo_get_merge_bases(the_repository,
  E, F);

@@
expression E;
expression F;
expression G;
@@
- get_merge_bases_many(
+ repo_get_merge_bases_many(the_repository,
  E, F, G);

@@
expression E;
expression F;
expression G;
@@
- get_merge_bases_many_dirty(
+ repo_get_merge_bases_many_dirty(the_repository,
  E, F, G);

@@
expression E;
expression F;
@@
- in_merge_bases(
+ repo_in_merge_bases(the_repository,
  E, F);

@@
expression E;
expression F;
expression G;
@@
- in_merge_bases_many(
+ repo_in_merge_bases_many(the_repository,
  E, F, G);

@@
expression E;
expression F;
@@
- get_cummit_buffer(
+ repo_get_cummit_buffer(the_repository,
  E, F);

@@
expression E;
expression F;
@@
- unuse_cummit_buffer(
+ repo_unuse_cummit_buffer(the_repository,
  E, F);

@@
expression E;
expression F;
expression G;
@@
- logmsg_reencode(
+ repo_logmsg_reencode(the_repository,
  E, F, G);

@@
expression E;
expression F;
expression G;
expression H;
@@
- format_cummit_message(
+ repo_format_cummit_message(the_repository,
  E, F, G, H);
