#!/bin/sh

systemctl $@ apache2 > /dev/null 2>&1
systemctl $@ httpd > /dev/null 2>&1
systemctl $@ postgresql > /dev/null 2>&1
systemctl $@ mariadb > /dev/null 2>&1
systemctl $@ mysql > /dev/null 2>&1
systemctl $@ nginx > /dev/null 2>&1
if [ -d /etc/uwsgi ]; then cd /etc/uwsgi; shopt -s nullglob; for f in *.ini; do f=$(basename "$f" .ini); systemctl $@ uwsgi@$f > /dev/null 2>&1; done; fi
