#!/bin/sh

test_description='git rebase --abort tests'

GIT_TEST_DEFAULT_INITIAL_BRANCH_NAME=main
export GIT_TEST_DEFAULT_INITIAL_BRANCH_NAME

. ./test-lib.sh

test_expect_success setup '
	test_cummit a a a &&
	git branch to-rebase &&

	test_cummit --annotate b a b &&
	test_cummit --annotate c a c &&

	git checkout to-rebase &&
	test_cummit "merge should fail on this" a d d &&
	test_cummit --annotate "merge should fail on this, too" a e pre-rebase
'

# Check that HEAD is equal to "pre-rebase" and the current branch is
# "to-rebase"
check_head() {
	test_cmp_rev HEAD pre-rebase^{cummit} &&
	test "$(git symbolic-ref HEAD)" = refs/heads/to-rebase
}

testrebase() {
	type=$1
	state_dir=$2

	test_expect_success "rebase$type --abort" '
		# Clean up the state from the previous one
		git reset --hard pre-rebase &&
		test_must_fail git rebase$type main &&
		test_path_is_dir "$state_dir" &&
		git rebase --abort &&
		check_head &&
		test_path_is_missing "$state_dir"
	'

	test_expect_success "rebase$type --abort after --skip" '
		# Clean up the state from the previous one
		git reset --hard pre-rebase &&
		test_must_fail git rebase$type main &&
		test_path_is_dir "$state_dir" &&
		test_must_fail git rebase --skip &&
		test_cmp_rev HEAD main &&
		git rebase --abort &&
		check_head &&
		test_path_is_missing "$state_dir"
	'

	test_expect_success "rebase$type --abort after --continue" '
		# Clean up the state from the previous one
		git reset --hard pre-rebase &&
		test_must_fail git rebase$type main &&
		test_path_is_dir "$state_dir" &&
		echo c > a &&
		echo d >> a &&
		git add a &&
		test_must_fail git rebase --continue &&
		test_cmp_rev ! HEAD main &&
		git rebase --abort &&
		check_head &&
		test_path_is_missing "$state_dir"
	'

	test_expect_success "rebase$type --abort when checking out a tag" '
		test_when_finished "git symbolic-ref HEAD refs/heads/to-rebase" &&
		git reset --hard a -- &&
		test_must_fail git rebase$type --onto b c pre-rebase &&
		test_cmp_rev HEAD b^{cummit} &&
		git rebase --abort &&
		test_cmp_rev HEAD pre-rebase^{cummit} &&
		! git symbolic-ref HEAD
	'

	test_expect_success "rebase$type --abort does not update reflog" '
		# Clean up the state from the previous one
		git reset --hard pre-rebase &&
		git reflog show to-rebase > reflog_before &&
		test_must_fail git rebase$type main &&
		git rebase --abort &&
		git reflog show to-rebase > reflog_after &&
		test_cmp reflog_before reflog_after &&
		rm reflog_before reflog_after
	'

	test_expect_success 'rebase --abort can not be used with other options' '
		# Clean up the state from the previous one
		git reset --hard pre-rebase &&
		test_must_fail git rebase$type main &&
		test_must_fail git rebase -v --abort &&
		test_must_fail git rebase --abort -v &&
		git rebase --abort
	'

	test_expect_success "rebase$type --quit" '
		test_when_finished "git symbolic-ref HEAD refs/heads/to-rebase" &&
		# Clean up the state from the previous one
		git reset --hard pre-rebase &&
		test_must_fail git rebase$type main &&
		test_path_is_dir $state_dir &&
		head_before=$(git rev-parse HEAD) &&
		git rebase --quit &&
		test_cmp_rev HEAD $head_before &&
		test_path_is_missing .git/rebase-apply
	'
}

testrebase " --apply" .git/rebase-apply
testrebase " --merge" .git/rebase-merge

test_done
