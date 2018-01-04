%define apxs    /usr/sbin/apxs2
%define apache_datadir          %(%{apxs} -q DATADIR)
%define apache_sysconfdir       %(%{apxs} -q SYSCONFDIR)

Name: allview
Version: 1.0 Release6
Release: 1
Group: System Environment/Daemons
License: GPL
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root
BuildPrereq: mysql-client, mysql-devel, ucdsnmp
Requires: mysql-client, ucdsnmp
Summary: A network monitor.

%define allview_prefix           /opt/%{name}
%define allview_bindir 		%{allview_prefix}/bin
%define allview_confdir 		%{_sysconfdir}/%{name}
%define allview_phpfrontend	%{allview_prefix}/frontends/php

%description
allview is a network monitor.

%package agent
Summary: Allview agent
Group: System Environment/Daemons

%description agent
the allview network monitor agent.

%package phpfrontend
Summary: Allview web frontend (php).
Group: System Environment/Daemons
Requires: php

%description phpfrontend
a php frontend for allview.

%prep
%setup -q

%build
%configure --with-mysql --with-ucd-snmp
make

# adjust in several files /home/allview
for allviewfile in misc/conf/* misc/init.d/suse/*/allview_agentd src/allview_server/{alerter,server}.c; do
  sed -i -e "s#/home/allview/bin#%{allview_bindir}#g" \
         -e "s#PidFile=/var/tmp#PidFile=%{_localstatedir}/run#g" \
         -e "s#LogFile=/tmp#LogFile=%{_localstatedir}/log#g" \
         -e "s#/home/allview/lock#%{_localstatedir}/lock#g" $allviewfile
done

# adjust /home/allview to /usr/share/doc/packages/
sed -i -e "s#/home/allview#%{_defaultdocdir}#g" create/data/images.sql

%pre
if [ -z "`grep allview etc/group`" ]; then
  usr/sbin/groupadd allview >/dev/null 2>&1
fi
if [ -z "`grep allview etc/passwd`" ]; then
  usr/sbin/useradd -g allview allview >/dev/null 2>&1
fi

%pre agent
if [ -z "`grep allview etc/group`" ]; then
  usr/sbin/groupadd allview >/dev/null 2>&1
fi
if [ -z "`grep allview etc/passwd`" ]; then
  usr/sbin/useradd -g allview allview >/dev/null 2>&1
fi

%post agent
%{fillup_and_insserv -f allview_agentd}

if [ -z "`grep allview_agent etc/services`" ]; then
  cat >>etc/services <<EOF
allview_agent	10050/tcp
EOF
fi

if [ -z "`grep allview_trap etc/services`" ]; then
  cat >>etc/services <<EOF
allview_trap	10051/tcp
EOF
fi

%postun agent
%{insserv_cleanup}

%clean
rm -fr $RPM_BUILD_ROOT

%install
rm -fr $RPM_BUILD_ROOT
#make install DESTDIR=$RPM_BUILD_ROOT

# create directory structure
install -d %{buildroot}%{allview_bindir}
install -d %{buildroot}%{allview_confdir}
install -d %{buildroot}%{_sysconfdir}/init.d
install -d %{buildroot}%{apache_sysconfdir}/conf.d

# copy binaries
install -m 755 bin/allview_* %{buildroot}%{allview_bindir}

# copy conf files
install -m 755 misc/conf/allview_*.conf %{buildroot}%{allview_confdir}

# copy frontends
cp -r frontends %{buildroot}%{allview_prefix}

# apache2 config
cat >allview.conf <<EOF
Alias /%{name} %{allview_phpfrontend}

<Directory "%{allview_phpfrontend}">
    Options FollowSymLinks
    AllowOverride None
    Order allow,deny
    Allow from all
</Directory>
EOF

install -m 644 allview.conf %{buildroot}%{apache_sysconfdir}/conf.d

# SuSE Start Scripts
install -m 755 misc/init.d/suse/9.1/allview_* %{buildroot}%{_sysconfdir}/init.d/

%files
%defattr(-,root,root)
%doc AUTHORS COPYING NEWS README INSTALL create upgrades
%dir %attr(0755,root,root) %{allview_confdir}
%attr(0644,root,root) %config(noreplace) %{allview_confdir}/allview_server.conf
%dir %attr(0755,root,root) %{allview_prefix}
%dir %attr(0755,root,root) %{allview_bindir}
%attr(0755,root,root) %{allview_bindir}/allview_server

%files agent 
%defattr(-,root,root)
%dir %attr(0755,root,root) %{allview_confdir}
%attr(0644,root,root) %config(noreplace) %{allview_confdir}/allview_agent.conf
%attr(0644,root,root) %config(noreplace) %{allview_confdir}/allview_agentd.conf
%config(noreplace) %{_sysconfdir}/init.d/allview_agentd
%dir %attr(0755,root,root) %{allview_prefix}
%dir %attr(0755,root,root) %{allview_bindir}
%attr(0755,root,root) %{allview_bindir}/allview_agent
%attr(0755,root,root) %{allview_bindir}/allview_agentd
%attr(0755,root,root) %{allview_bindir}/allview_sender

%files phpfrontend
%defattr(-,root,root)
%attr(0644,root,root) %config(noreplace) %{apache_sysconfdir}/conf.d/allview.conf
%dir %attr(0755,root,root) %{allview_prefix}
%dir %attr(0755,root,root) %{allview_prefix}/frontends
%attr(0755,root,root) %{allview_phpfrontend}

%changelog
* Sun Mar 07 2006 Myleft Studio <dreamsxin@qq.com>
- update to 1.1

%changelog
* Fri Jan 29 2005 Dirk Datzert <dirk@datzert.de>
- update to 1.1aplha6

* Tue Jun 01 2003 Allview team  <allview@gobbo.caves.lv>
- update to 1.0beta10 

* Tue Jun 01 2003 Harald Holzer <hholzer@may.co.at>
- update to 1.0beta9
- move phpfrontend config to /etc/allview

* Tue May 23 2003 Harald Holzer <hholzer@may.co.at>
- split the php frontend in a extra package

* Tue May 20 2003 Harald Holzer <hholzer@may.co.at>
- 1.0beta8
- initial packaging
