%define tempdocdir %{_prefix}/elektra

Name:          elektra
Version:       0.6.4
Release:       1
Source:        http://prdownloads.sourceforge.net/elektra/%{name}-%{version}.tar.gz
Group:         System Environment/Libraries
License:       BSD
URL:           http://www.libelektra.org
# Requires:      libxml2
BuildRoot:     %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: doxygen db4-devel libxml2-devel docbook-style-xsl libxslt

# Requires section:
# libxml2: used only by libkdbtools, to import keys from XML
#
# BuildRequires section:
# doxygen: to build the API documentation
# libxslt, docbook-style-xsl: to build man pages from docbook with xsltproc program
# db4-devel: for the Berkeley DB backend
# libxml2-devel: for the kdbtools library build
# gettext-devel: for the autoconf ICONV macro only
# automake, autoconf: for the build system
# libtool: not sure
# GConf2-devel: for the testing GConf2 backend for Elektra


Summary:       A key/value pair database to store software configurations



%description
Elektra provides a universal and secure framework to store configuration
parameters in a hierarchical key-value pair mechanism, instead of each
program using its own text configuration files. This allows any program
to read and save its configuration with a consistent API, and allows
them to be aware of other applications' configurations, permitting
easy application integration. While architecturally similar to other OS
registries, Elektra does not have most of the problems found in those
implementations.

%package devel
Summary:      Include files and API documentation to build elektrified programs
Group:        Development/System
Requires:     pkgconfig elektra = %{version}-%{release}

%description devel
Elektra provides a universal and secure framework to store configuration
parameters in a hierarchical key-value pair tree.

This package contains development specific things as include files and
static libraries to create elektrified programs.


#%package backend-gconf
#Summary:      A GConf backend for Elektra
#Group:        System Environment/Libraries
#Requires:     elektra
#add this in the main RPM description: BuildRequires: GConf2-devel


#%description backend-gconf
#Elektra provides a universal and secure framework to store configuration
#parameters in a hierarchical key-value pair mechanism, instead of each
#program using its own text configuration files. This allows any program
#to read and save its configuration with a consistent API, and allows
#them to be aware of other applications' configurations, permitting
#easy application integration. While architecturally similar to other OS
#registries, Elektra does not have most of the problems found in those
#implementations.

#This package contains a GConf backend for Elektra, to let Elektra use a GConf
#daemon to store its keys.


%package backend-berkeleydb
Summary:      Elektra backend that stores key/value pairs in berkeley DB databases
Group:        System Environment/Libraries
Requires:     elektra


%description backend-berkeleydb
Elektra provides a universal and secure framework to store configuration
parameters in a hierarchical key-value pair tree.

This package contains a Berkeley DB backend for Elektra, to let Elektra use
Berkeley DB databases to store its keys.

# Uncomment and join the '%' char to 'debug_package' if you want to build debug RPMs
# % debug_package

%prep
%setup -q
%build

#CFLAGS="-g -O0" \

%configure \
     --bindir=/bin \
     --sbindir=/sbin \
     --libdir=/%{_lib} \
     --with-docdir=%{tempdocdir}/elektra-%{version} \
     --with-develdocdir=%{tempdocdir}/elektra-devel-%{version} \
     --with-docbook=%{_datadir}/sgml/docbook/xsl-stylesheets

#     --prefix=%{_prefix} \
#     --exec-prefix=/ \


# Add these options if using a plain ./configure instead of RPM's % configure macro
# --sysconfdir=%{_sysconfdir} \
# --mandir=%{_mandir}

make %{?_smp_mflags} all


%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

# Remove statically linked kdb
rm $RPM_BUILD_ROOT/bin/kdb_static

# Move .a files to -devel package
mv $RPM_BUILD_ROOT/%{_lib}/libelektra.a $RPM_BUILD_ROOT%{_libdir}

# Prepare devel files
#mv $RPM_BUILD_ROOT/%{_lib}/libelektra.so $RPM_BUILD_ROOT%{_libdir}/
rm $RPM_BUILD_ROOT/%{_lib}/libelektra.so
ln -sf ../../%{_lib}/libelektra.so.2 $RPM_BUILD_ROOT%{_libdir}/libelektra.so
#rm $RPM_BUILD_ROOT/%{_lib}/libelektra.so

# Remove .la files
#rm $RPM_BUILD_ROOT/usr/lib/libelektra-*.a
rm $RPM_BUILD_ROOT/%{_lib}/*.la
rm $RPM_BUILD_ROOT/%{_lib}/elektra/*.la
rm $RPM_BUILD_ROOT%{_libdir}/*.la

# Remove a file that conflicts with other packages
rm $RPM_BUILD_ROOT%{_mandir}/man3/key.3*

# Remove documentation from 'make install', to let RPM package it alone
rm -rf $RPM_BUILD_ROOT%{tempdocdir}
rm -rf scripts/Makefile*
rm -rf examples/Makefile*
rm -rf examples/.deps
rm -rf doc/standards/Makefile*
mv doc/elektra-api/html doc/elektra-api/api-html




%clean
rm -rf $RPM_BUILD_ROOT
#rm -rf $RPM_BUILD_DIR/%{name}




%post
/sbin/ldconfig
# Backwards compatibility, from the Linux Registry days
#if [ -d /etc/registry -a ! -d /etc/kdb ]; then
#   $MOVE /etc/registry /etc/kdb
#   $LINK -s kdb /etc/registry
#fi

# Create basic key structure for apps
kdb set -t dir system/sw
kdb set system/sw/kdb/schemapath "%{_datadir}/sgml/elektra-0.1.1/elektra.xsd"



%postun -p /sbin/ldconfig



%files
%defattr(-,root,root,-)
/bin/*
%attr(4755,root,root) /sbin/kdbd
/%{_lib}/*elektra.so*
%dir /%{_lib}/elektra
/%{_lib}/elektra/*elektra-daemon.so*
/%{_lib}/elektra/*elektra-filesys.so*
/%{_lib}/elektra/*elektra-default.so*
/%{_lib}/elektra/*elektra-ddefault.so*
/%{_lib}/elektra/*elektra-fstab.so*
#/%{_lib}/elektra/*elektra-ini.so*
%{_libdir}/*elektratools.so.*
%{_libdir}/elektra/*elektratools.so
%{_sysconfdir}/profile.d/*
%doc AUTHORS COPYING ChangeLog README INSTALL
%doc scripts doc/standards
%{_mandir}/man1/*
%{_mandir}/man7/*
%{_mandir}/man5/*
%{_datadir}/sgml/*


%files devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/*.a
%{_libdir}/libelektra.so
%{_libdir}/libelektratools.so
%{_libdir}/pkgconfig/*
%doc examples doc/elektra-api/api-html
%{_mandir}/man3/*



#%files backend-gconf
#%defattr(-,root,root,-)
#%{_libdir}/elektra/*gconf.so*

%files backend-berkeleydb
%defattr(-,root,root,-)
/%{_lib}/elektra/*berkeleydb.so*



%changelog
* Thu Sep 07 2006 Avi Alkalay <avi@unix.sh> 0.6.4-1
- Version update
* Thu Jun 02 2006 Avi Alkalay <avi@unix.sh> 0.6.2-1
- Initial packaging

