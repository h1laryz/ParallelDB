
import psycopg2

conn = psycopg2.connect(
    host="127.0.0.1",
    database="kursach",
    user="postgres",
    password="12345")
cur = conn.cursor()
sql = "TRUNCATE table small.anime_list RESTART IDENTITY;"

cur.execute(sql)
conn.commit()

cur.close()
conn.close()
