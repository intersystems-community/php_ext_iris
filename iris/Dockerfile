FROM store/intersystems/iris:2019.1.0.511.0-community

COPY src /opt/php_iris/

# Install and Configure Apache+PHP+php_iris
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

ARG USER=demo
ARG PASS=demo
ARG NAMESPACE=DEMO

# Configure IRIS
RUN iris start $ISC_PACKAGE_INSTANCENAME quietly EmergencyId=sys,sys && \
    /bin/echo -e "sys\nsys\n" \
            " Do ##class(Security.Users).UnExpireUserPasswords(\"*\")\n" \
            " Do CreateDatabase^%SYS.SQLSEC(\"${NAMESPACE}\",\"\",,0)\n" \
            " Do ##Class(Security.Users).Create(\"${USER}\",\"%DB_%DEFAULT\",\"${PASS}\",,\"${NAMESPACE}\")\n" \
            " Set p(\"Enabled\") = 1\n" \
            " Do ##class(Security.Services).Modify(\"%Service_CallIn\", .p)\n" \
            " Halt" \
    | iris session $ISC_PACKAGE_INSTANCENAME && \
    /bin/echo -e "sys\nsys\n" \
    | iris stop $ISC_PACKAGE_INSTANCENAME quietly

COPY --chown=www-data:www-data php/demo /var/www/html

RUN echo "service apache2 start" > /startApache.sh && chmod +x /startApache.sh

CMD [ "-a", "/startApache.sh" ]