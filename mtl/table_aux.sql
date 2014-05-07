CREATE TABLE memory (
    id SERIAL,
    statu int NOT NULL DEFAULT 0,

    title varchar(256) NOT NULL DEFAULT '',
    hint varchar(256) NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',

    previd int NOT NULL DEFAULT 0,
    nextid int NOT NULL DEFAULT 0,
    
    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);
CREATE TRIGGER tg_uptime_memory BEFORE UPDATE ON memory FOR EACH ROW EXECUTE PROCEDURE update_time();

CREATE TABLE comment (
    id SERIAL,

    type int NOT NULL DEFAULT 0, --0 memory
    statu int NOT NULL DEFAULT 0,

    oid int NOT NULL DEFAULT 0, --object id
    otitle varchar(256) NOT NULL DEFAULT '', --object title
    pid int NOT NULL DEFAULT 0,

    ip varchar(64) NOT NULL DEFAULT '',
    addr varchar(256) NOT NULL DEFAULT '',

    author varchar(64) NOT NULL DEFAULT '',
    contact varchar(256) NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',

    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);
CREATE TRIGGER tg_uptime_comment BEFORE UPDATE ON comment FOR EACH ROW EXECUTE PROCEDURE update_time();

CREATE TABLE email (
    id SERIAL,
    statu int NOT NULL DEFAULT 0, -- 0 fresh 1 sended
    gotime int NOT NULL DEFAULT 0, -- 0 immediatly 1 minute 2 O clock 3 midnight
    opts varchar(512) NOT NULL DEFAULT '', -- -c /usr/local/etc/email/liuchunsheng.conf -html -s....
    subject varchar(256) NOT NULL DEFAULT '',
    sendto text NOT NULL DEFAULT '',
    cc text NOT NULL DEFAULT '',
    bcc text NOT NULL DEFAULT '',
    content text NOT NULL DEFAULT '',
    checksum varchar(64) NOT NULL DEFAULT '',
    intime timestamp DEFAULT now(),
    uptime timestamp DEFAULT now(),
    PRIMARY KEY (id)
);

CREATE INDEX email_index ON email (statu, gotime);
CREATE TRIGGER tg_uptime_email BEFORE UPDATE ON email FOR EACH ROW EXECUTE PROCEDURE update_time();

-- OWN Aggregate Functions
CREATE AGGREGATE str_concat(
  basetype    = text,
  sfunc       = textcat,
  stype       = text,
  initcond    = ''
);

CREATE TABLE inbox (
	id SERIAL,
    type smallint NOT NULL DEFAULT 0, -- 0 system, 1 private
    statu smallint NOT NULL DEFAULT 0,
    mid int NOT NULL DEFAULT 0,
    fresh smallint NOT NULL DEFAULT 1,
    fmid int NOT NULL DEFAULT 0,
    fnick varchar(256) NOT NULL DEFAULT '',
    message varchar(1024) NOT NULL DEFAULT '',
    intime timestamp DEFAULT now()
);
