1.FATAL:  password authentication failed for user "postgres"
	postgres=# ALTER ROLE postgres with password '4321';
2.sudo -u postgres psql	//into psql
3.find attname:
SELECT a.attnum,                          
       a.attname AS field
  FROM pg_class c,
       pg_attribute a
 WHERE c.relname = 'test1'
       and a.attnum > 0
       and a.attrelid = c.oid
 ORDER BY a.attnum;
4.