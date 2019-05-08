FROM store/intersystems/iris:2019.1.0.511.0-community

COPY src /opt/php_iris/

RUN apt -y update \
 && DEBIAN_FRONTEND=noninteractive apt -y install apache2 libapache2-mod-php php-dev \
 && apt clean \
 && sed -i '1iServerName localhost' /etc/apache2/apache2.conf \
 && usermod -a -G irisusr www-data \
 && cd /opt/php_iris \
 && phpize \
 && ./configure --with-path=${ISC_PACKAGE_INSTALLDIR}/bin \
 && make \
 && make install \
 && echo 'extension=iris.so' > /etc/php/7.2/mods-available/iris.ini \
 && phpenmod iris

COPY --chown=www-data:www-data php/demo /var/www/html

COPY startApache.sh /

CMD [ "-a", "/startApache.sh" ]