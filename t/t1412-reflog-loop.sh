#!/bin/sh

test_description='reflog walk shows repeated cummits again'
. ./test-lib.sh

test_expect_success 'setup cummits' '
	test_cummit one file content &&
	test_cummit --append two file content
'

test_expect_success 'setup reflog with alternating cummits' '
	git checkout -b topic &&
	git reset one &&
	git reset two &&
	git reset one &&
	git reset two
'

test_expect_success 'reflog shows all entries' '
	cat >expect <<-\EOF &&
		topic@{0} reset: moving to two
		topic@{1} reset: moving to one
		topic@{2} reset: moving to two
		topic@{3} reset: moving to one
		topic@{4} branch: Created from HEAD
	EOF
	git log -g --format="%gd %gs" topic >actual &&
	test_cmp expect actual
'

test_done
