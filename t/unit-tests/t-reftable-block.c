/*
Copyright 2020 Google LLC

Use of this source code is governed by a BSD-style
license that can be found in the LICENSE file or at
https://developers.google.com/open-source/licenses/bsd
*/

#include "test-lib.h"
#include "reftable/block.h"
#include "reftable/blocksource.h"
#include "reftable/constants.h"
#include "reftable/reftable-error.h"

static void t_ref_block_read_write(void)
{
	const int header_off = 21; /* random */
	struct reftable_record recs[30];
	const size_t N = ARRAY_SIZE(recs);
	const size_t block_size = 1024;
	struct reftable_block block = { 0 };
	struct block_writer bw = {
		.last_key = STRBUF_INIT,
	};
	struct reftable_record rec = {
		.type = BLOCK_TYPE_REF,
	};
	size_t i = 0;
	int n;
	struct block_reader br = { 0 };
	struct block_iter it = BLOCK_ITER_INIT;
	struct strbuf want = STRBUF_INIT;

	REFTABLE_CALLOC_ARRAY(block.data, block_size);
	block.len = block_size;
	block.source = malloc_block_source();
	block_writer_init(&bw, BLOCK_TYPE_REF, block.data, block_size,
			  header_off, hash_size(GIT_SHA1_FORMAT_ID));

	rec.u.ref.refname = (char *) "";
	rec.u.ref.value_type = REFTABLE_REF_DELETION;
	n = block_writer_add(&bw, &rec);
	check_int(n, ==, REFTABLE_API_ERROR);

	for (i = 0; i < N; i++) {
		rec.u.ref.refname = xstrfmt("branch%02"PRIuMAX , (uintmax_t)i);
		rec.u.ref.value_type = REFTABLE_REF_VAL1;
		memset(rec.u.ref.value.val1, i, GIT_SHA1_RAWSZ);

		recs[i] = rec;
		n = block_writer_add(&bw, &rec);
		rec.u.ref.refname = NULL;
		rec.u.ref.value_type = REFTABLE_REF_DELETION;
		check_int(n, ==, 0);
	}

	n = block_writer_finish(&bw);
	check_int(n, >, 0);

	block_writer_release(&bw);

	block_reader_init(&br, &block, header_off, block_size, GIT_SHA1_RAWSZ);

	block_iter_seek_start(&it, &br);

	for (i = 0; ; i++) {
		int r = block_iter_next(&it, &rec);
		check_int(r, >=, 0);
		if (r > 0)
			break;
		check(reftable_record_equal(&recs[i], &rec, GIT_SHA1_RAWSZ));
	}

	for (i = 0; i < N; i++) {
		block_iter_reset(&it);
		reftable_record_key(&recs[i], &want);

		n = block_iter_seek_key(&it, &br, &want);
		check_int(n, ==, 0);

		n = block_iter_next(&it, &rec);
		check_int(n, ==, 0);

		check(reftable_record_equal(&recs[i], &rec, GIT_SHA1_RAWSZ));

		want.len--;
		n = block_iter_seek_key(&it, &br, &want);
		check_int(n, ==, 0);

		n = block_iter_next(&it, &rec);
		check_int(n, ==, 0);
		check(reftable_record_equal(&recs[10 * (i / 10)], &rec, GIT_SHA1_RAWSZ));
	}

	block_reader_release(&br);
	block_iter_close(&it);
	reftable_record_release(&rec);
	reftable_block_done(&br.block);
	strbuf_release(&want);
	for (i = 0; i < N; i++)
		reftable_record_release(&recs[i]);
}

static void t_log_block_read_write(void)
{
	const int header_off = 21;
	struct reftable_record recs[30];
	const size_t N = ARRAY_SIZE(recs);
	const size_t block_size = 2048;
	struct reftable_block block = { 0 };
	struct block_writer bw = {
		.last_key = STRBUF_INIT,
	};
	struct reftable_record rec = {
		.type = BLOCK_TYPE_LOG,
	};
	size_t i = 0;
	int n;
	struct block_reader br = { 0 };
	struct block_iter it = BLOCK_ITER_INIT;
	struct strbuf want = STRBUF_INIT;

	REFTABLE_CALLOC_ARRAY(block.data, block_size);
	block.len = block_size;
	block.source = malloc_block_source();
	block_writer_init(&bw, BLOCK_TYPE_LOG, block.data, block_size,
			  header_off, hash_size(GIT_SHA1_FORMAT_ID));

	for (i = 0; i < N; i++) {
		rec.u.log.refname = xstrfmt("branch%02"PRIuMAX , (uintmax_t)i);
		rec.u.log.update_index = i;
		rec.u.log.value_type = REFTABLE_LOG_UPDATE;

		recs[i] = rec;
		n = block_writer_add(&bw, &rec);
		rec.u.log.refname = NULL;
		rec.u.log.value_type = REFTABLE_LOG_DELETION;
		check_int(n, ==, 0);
	}

	n = block_writer_finish(&bw);
	check_int(n, >, 0);

	block_writer_release(&bw);

	block_reader_init(&br, &block, header_off, block_size, GIT_SHA1_RAWSZ);

	block_iter_seek_start(&it, &br);

	for (i = 0; ; i++) {
		int r = block_iter_next(&it, &rec);
		check_int(r, >=, 0);
		if (r > 0)
			break;
		check(reftable_record_equal(&recs[i], &rec, GIT_SHA1_RAWSZ));
	}

	for (i = 0; i < N; i++) {
		block_iter_reset(&it);
		strbuf_reset(&want);
		strbuf_addstr(&want, recs[i].u.log.refname);

		n = block_iter_seek_key(&it, &br, &want);
		check_int(n, ==, 0);

		n = block_iter_next(&it, &rec);
		check_int(n, ==, 0);

		check(reftable_record_equal(&recs[i], &rec, GIT_SHA1_RAWSZ));

		want.len--;
		n = block_iter_seek_key(&it, &br, &want);
		check_int(n, ==, 0);

		n = block_iter_next(&it, &rec);
		check_int(n, ==, 0);
		check(reftable_record_equal(&recs[10 * (i / 10)], &rec, GIT_SHA1_RAWSZ));
	}

	block_reader_release(&br);
	block_iter_close(&it);
	reftable_record_release(&rec);
	reftable_block_done(&br.block);
	strbuf_release(&want);
	for (i = 0; i < N; i++)
		reftable_record_release(&recs[i]);
}

static void t_obj_block_read_write(void)
{
	const int header_off = 21;
	struct reftable_record recs[30];
	const size_t N = ARRAY_SIZE(recs);
	const size_t block_size = 1024;
	struct reftable_block block = { 0 };
	struct block_writer bw = {
		.last_key = STRBUF_INIT,
	};
	struct reftable_record rec = {
		.type = BLOCK_TYPE_OBJ,
	};
	size_t i = 0;
	int n;
	struct block_reader br = { 0 };
	struct block_iter it = BLOCK_ITER_INIT;
	struct strbuf want = STRBUF_INIT;

	REFTABLE_CALLOC_ARRAY(block.data, block_size);
	block.len = block_size;
	block.source = malloc_block_source();
	block_writer_init(&bw, BLOCK_TYPE_OBJ, block.data, block_size,
			  header_off, hash_size(GIT_SHA1_FORMAT_ID));

	for (i = 0; i < N; i++) {
		uint8_t *bytes = reftable_malloc(sizeof(uint8_t[5]));
		memcpy(bytes, (uint8_t[]){i, i+1, i+2, i+3, i+5}, sizeof(uint8_t[5]));

		rec.u.obj.hash_prefix = bytes;
		rec.u.obj.hash_prefix_len = 5;

		recs[i] = rec;
		n = block_writer_add(&bw, &rec);
		rec.u.obj.hash_prefix = NULL;
		rec.u.obj.hash_prefix_len = 0;
		check_int(n, ==, 0);
	}

	n = block_writer_finish(&bw);
	check_int(n, >, 0);

	block_writer_release(&bw);

	block_reader_init(&br, &block, header_off, block_size, GIT_SHA1_RAWSZ);

	block_iter_seek_start(&it, &br);

	for (i = 0; ; i++) {
		int r = block_iter_next(&it, &rec);
		check_int(r, >=, 0);
		if (r > 0)
			break;
		check(reftable_record_equal(&recs[i], &rec, GIT_SHA1_RAWSZ));
	}

	for (i = 0; i < N; i++) {
		block_iter_reset(&it);
		reftable_record_key(&recs[i], &want);

		n = block_iter_seek_key(&it, &br, &want);
		check_int(n, ==, 0);

		n = block_iter_next(&it, &rec);
		check_int(n, ==, 0);

		check(reftable_record_equal(&recs[i], &rec, GIT_SHA1_RAWSZ));
	}

	block_reader_release(&br);
	block_iter_close(&it);
	reftable_record_release(&rec);
	reftable_block_done(&br.block);
	strbuf_release(&want);
	for (i = 0; i < N; i++)
		reftable_record_release(&recs[i]);
}

int cmd_main(int argc, const char *argv[])
{
	TEST(t_log_block_read_write(), "read-write operations on log blocks work");
	TEST(t_obj_block_read_write(), "read-write operations on obj blocks work");
	TEST(t_ref_block_read_write(), "read-write operations on ref blocks work");

	return test_done();
}
