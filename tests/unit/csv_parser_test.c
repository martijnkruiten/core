#include <test.h>

#include <sequence.h>
#include <alloc.h>
#include <csv_parser.h>
#include <writer.h>

static void test_new_csv_reader_empty_string()
{
    char line[]=",Null is not empty string,,\"\",\"\"";

    Seq *list = SeqParseCsvString(line);
    assert_true(list != NULL);
    assert_int_equal(list->length, 5);
    assert_true(list->data[0] != NULL);
    assert_true(list->data[1] != NULL);
    assert_true(list->data[2] != NULL);
    assert_true(list->data[3] != NULL);
    assert_true(list->data[4] != NULL);
    assert_string_equal(list->data[0], "");
    assert_string_equal(list->data[1], "Null is not empty string");
    assert_string_equal(list->data[2], "");
    assert_string_equal(list->data[3], "");
    assert_string_equal(list->data[4], "");
    assert_string_equal(list->data[2], list->data[3]);
    SeqDestroy(list);
}

static void test_new_csv_reader_basic()
{
    Seq *list = NULL;
    char *lines[5] = {
        "aaaa,bbb,ccccc",
        "\"aaaa\",\"bbb\",\"ccccc\"",
        "\"aaaa\",bbb,\"ccccc\"",
        "aaaa,\"bbb\",ccccc",
        ",\"bbb\",ccccc",
    };


    for (int i=0; i<5; i++)
    {
        list = SeqParseCsvString(lines[i]);
        assert_int_equal(list->length, 3);
        assert_string_equal(list->data[2], "ccccc");
        if (list != NULL)
        {
            SeqDestroy(list);
        } else {
            assert_true(false);
        }
    }
}

static void test_new_csv_reader()
{
    Seq *list = NULL;
    char line[]="  Type    ,\"walo1\",  \"walo2\"   ,  \"wal,o \"\" 3\",  \"  ab,cd  \", walo solo ,, \"walo\"";


    list = SeqParseCsvString(line);

    assert_int_equal(list->length, 8);
    assert_string_equal(list->data[3], "wal,o \" 3");
    assert_string_equal(list->data[6], "");
    assert_string_equal(list->data[7], "walo");
    if (list != NULL)
    {
        SeqDestroy(list);
    }
    else
    {
        assert_true(false);
    }
}

static void test_new_csv_reader_lfln()
{
    Seq *list = NULL;
    char line[]="  Type    ,\"walo1\",  \"walo2\"   ,  \"wa\r\nl,o\n \"\"\r 3\",  \"  ab,cd  \", walo solo ,, \"walo\" ";


    list = SeqParseCsvString(line);

    assert_int_equal(list->length, 8);
    assert_string_equal(list->data[3], "wa\r\nl,o\n \"\r 3");
    if (list != NULL)
    {
        SeqDestroy(list);
    }
    else
    {
        assert_true(false);
    }
}

static void test_new_csv_reader_lfln_at_end()
{
    Seq *list = NULL;
    char line[]="  Type    ,\"walo1\",  \"walo2\"   ,  \"wal\r\n,o \"\" 3\",  \"  ab,cd  \", walo solo , , \"walo\"       \r\n           ";


    list = SeqParseCsvString(line);

    assert_int_equal(list->length, 8);
    assert_string_equal(list->data[3], "wal\r\n,o \" 3");
    assert_string_equal(list->data[6], " ");
    assert_string_equal(list->data[7], "walo");
    if (list != NULL)
    {
        SeqDestroy(list);
    }
    else
    {
        assert_true(false);
    }
}

static void test_new_csv_reader_lfln_at_end2()
{
    Seq *list = NULL;
    char line[]="  Type    ,\"walo1\",  \"walo2\"   ,  \"wal\r\n,o \"\" 3\",  \"  ab,cd  \", walo solo , ,walo\r\n";

    list = SeqParseCsvString(line);

    assert_int_equal(list->length, 8);
    assert_string_equal(list->data[7], "walo");
    if (list != NULL)
    {
        SeqDestroy(list);
    }
    else
    {
        assert_true(false);
    }
}

static void test_new_csv_reader_lfln_at_end3()
{
    Seq *list = NULL;
    char line[]="  Type    ,\"walo1\",  \"walo2\"   ,  \"wal\r\n,o \"\" 3\",  \"  ab,cd  \", walo solo , , \r\n";


    list = SeqParseCsvString(line);

    assert_int_equal(list->length, 8);
    assert_string_equal(list->data[7], " ");
    if (list != NULL)
    {
        SeqDestroy(list);
    }
    else
    {
        assert_true(false);
    }
}

static void test_get_next_line()
{
    FILE *fp = fopen("./data/csv_file.csv", "r");
    assert_true(fp);

    {
        char *line = GetCsvLineNext(fp);
        assert_true(line);
        assert_string_equal(line, "field_1, field_2\r\n");

        Seq *list = SeqParseCsvString(line);
        assert_true(list);
        assert_int_equal(SeqLength(list), 2);
        assert_string_equal(SeqAt(list, 0), "field_1");
        assert_string_equal(SeqAt(list, 1), " field_2");
        SeqDestroy(list);
        free(line);
    }

    {
        char *line = GetCsvLineNext(fp);
        assert_true(line);
        assert_string_equal(line, "field_1, \"value1 \nvalue2 \nvalue3\"\r\n");

        Seq *list = SeqParseCsvString(line);
        assert_true(list);
        assert_int_equal(SeqLength(list), 2);
        assert_string_equal(SeqAt(list, 0), "field_1");
        assert_string_equal(SeqAt(list, 1), "value1 \nvalue2 \nvalue3");
        SeqDestroy(list);
        free(line);
    }

    {
        char *line = GetCsvLineNext(fp);
        assert_true(line);
        assert_string_equal(line, "field_1, \"field,2\"\r\n");
        Seq *list = SeqParseCsvString(line);
        assert_true(list);
        assert_int_equal(SeqLength(list), 2);
        assert_string_equal(SeqAt(list, 0), "field_1");
        assert_string_equal(SeqAt(list, 1), "field,2");
        SeqDestroy(list);
        free(line);
    }

    fclose(fp);
}

/* A memory corruption (off-by-one write) used to occur with this string,
 * detectable by valgrind and crashing some non-linux platforms. */
static void test_new_csv_reader_zd3151_ENT3023()
{
    const char *input = "default.cfe_autorun_inventory_proc.cpuinfo_array[337][0";
    Seq *list = SeqParseCsvString(input);
    assert_int_equal(SeqLength(list), 1);
    assert_string_equal(input, SeqAt(list, 0));
    SeqDestroy(list);
}

static void test_new_csv_reader_carriage_return()
{
    const char *inputs[] =
    {
        "field_1,stuff,values are here\r\n",
        "field_2,empty,\r\n",
        "field_3,more stuff,more values are here\r\n",
        "field_4,,\r\n",
    };

    // First entry
    Seq *list = SeqParseCsvString(inputs[0]);
    assert_int_equal(SeqLength(list), 3);
    assert_string_equal(SeqAt(list, 2), "values are here");
    SeqDestroy(list);

    // Second entry
    list = SeqParseCsvString(inputs[1]);
    assert_int_equal(SeqLength(list), 3);
    assert_string_equal(SeqAt(list, 2), "");
    SeqDestroy(list);

    // Third entry
    list = SeqParseCsvString(inputs[2]);
    assert_int_equal(SeqLength(list), 3);
    assert_string_equal(SeqAt(list, 2), "more values are here");
    SeqDestroy(list);

    // Fourth entry
    list = SeqParseCsvString(inputs[3]);
    assert_int_equal(SeqLength(list), 3);
    assert_string_equal(SeqAt(list, 2), "");
    SeqDestroy(list);
}

int main()
{
    PRINT_TEST_BANNER();
    const UnitTest tests[] =
    {
        unit_test(test_new_csv_reader_empty_string),
        unit_test(test_new_csv_reader_basic),
        unit_test(test_new_csv_reader),
        unit_test(test_new_csv_reader_lfln),
        unit_test(test_new_csv_reader_lfln_at_end),
        unit_test(test_new_csv_reader_lfln_at_end2),
        unit_test(test_new_csv_reader_lfln_at_end3),
        unit_test(test_get_next_line),
        unit_test(test_new_csv_reader_zd3151_ENT3023),
        unit_test(test_new_csv_reader_carriage_return),
    };

    return run_tests(tests);
}

