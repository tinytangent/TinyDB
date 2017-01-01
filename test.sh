rm test_sqlite3.db
rm -rf test_tinydb
rm -rf TestResult

mkdir test_tinydb
mkdir TestResult

#/c/Applications/sqlite3/sqlite3.exe test_sqlite3.db < Tests/test_00_basic_sqlite.sql > TestResult/test_00_basic_sqlite.result
build/Release/TinyDB.exe test_tinydb Tests/test_00_basic_tinydb.sql > TestResult/test_00_basic_tinydb.result

#/c/Applications/sqlite3/sqlite3.exe test_sqlite3.db < Tests/test_01_single_table_level1_sqlite.sql > TestResult/test_01_single_table_level1_sqlite.result
build/Release/TinyDB.exe test_tinydb Tests/test_01_single_table_level1_tinydb.sql > TestResult/test_01_single_table_level1_tinydb.result

rm test_sqlite3.db
rm -rf test_tinydb
