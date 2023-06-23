import random
import psycopg2
import string

def get_random_string():
    length = random.randint(1, 50)
    # choose from all lowercase letter
    letters = string.ascii_lowercase
    result_str = ''.join(random.choice(letters) for i in range(length))
    return result_str

conn = psycopg2.connect(
    host="127.0.0.1",
    database="kursach",
    user="postgres",
    password="12345")
cur = conn.cursor()

for i in range(10000):
    sql = "INSERT INTO premedium.anime_list(name) VALUES('{}')"
    sql = sql.format(get_random_string())
    cur.execute(sql)
    conn.commit()
cur.close()
conn.close()
