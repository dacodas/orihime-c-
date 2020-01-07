FROM docker.io/mysql

ENV ORIHIME_DATA_ROOT_PATH=/docker-entrypoint-initdb.d/data

COPY src/migrations/ /docker-entrypoint-initdb.d/

RUN ln -s /docker-entrypoint-initdb.d/00-initial-schema \
       /docker-entrypoint-initdb.d/00-initial-schema.sh \
       && echo "bind-address = 0.0.0.0" >> /etc/mysql/conf.d/docker.cnf
