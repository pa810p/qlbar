Summary: Toolbar gadget for X11
Name: qlbar
Version: 0.2.5
Release: 20180421
License: GPL
Group: Applications/Productivity
URL: http://prokop.uek.krakow.pl/projects/qlbar
Source: http://prokop.uek.krakow.pl/projects/download/%{name}-%{version}.tar.bz2
Vendor: Pawel Prokop
Prefix: /
#BuildRoot: BUILDROOT 
#%{_builddir}
Requires: imlib2

%description
Toolbar gadget for X11

%prep
%setup

%build
%configure 
#--with-log4cxx
            
make DESTDIR=%{_buildrootdir} INSTALL="install -p" CP="cp -p" install

%install


%files


