#!/bin/sh

systemctl $@ apache2
systemctl $@ httpd
systemctl $@ postgresql
systemctl $@ mariadb
systemctl $@ mysql
systemctl $@ nginx
