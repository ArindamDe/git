#!/bin/sh

test_description='test case exclude pathspec'

. ./test-lib.sh

test_expect_success 'setup a submodule' '
	test_create_repo pretzel &&
	: >pretzel/a &&
	git -C pretzel add a &&
	git -C pretzel cummit -m "add a file" -- a &&
	git submodule add ./pretzel sub &&
	git cummit -a -m "add submodule" &&
	git submodule deinit --all
'

cat <<EOF >expect
fatal: Pathspec 'sub/a' is in submodule 'sub'
EOF

test_expect_success 'error message for path inside submodule' '
	echo a >sub/a &&
	test_must_fail git add sub/a 2>actual &&
	test_cmp expect actual
'

test_expect_success 'error message for path inside submodule from within submodule' '
	test_must_fail git -C sub add . 2>actual &&
	test_i18ngrep "in unpopulated submodule" actual
'

test_done
