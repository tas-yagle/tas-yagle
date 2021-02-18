
Name:           tasyag
Version:        3.4p5
Release:        1%{?dist}
Summary:        Tas/Yagle - Static Timing Analyser
Group:          Applications/Engineering
License:        UPMC Non-Commercial
URL:            https://soc-extras.lip6.fr/en/alliance-abstract-en/
Source0:        %{name}-%{version}.tar.bz2
Packager:       Jean-Paul Chaput <Jean-Paul.Chaput@lip6.fr>
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:  texlive-scheme-tetex
BuildRequires:  swig >= 1.3.27
BuildRequires:  tcl-devel >= 8.5.3
BuildRequires:  openmotif-devel
BuildRequires:  openmotif22
BuildRequires:  libXt
BuildRequires:  libXp
BuildRequires:  libXpm
BuildRequires:  java-1.6.0-openjdk
BuildRequires:  fop >= 0.95 
BuildRequires:  saxon9 >= 9.3.0.4
BuildRequires:  libedit-devel >= 2.11
Requires:       libedit >= 2.11


%description
STATIC TIMING ANALYSIS
  The  advent   of  semiconductor  fabrication  technologies   now  allows  high
performance in complex integrated circuits.
  With the increasing complexity of these circuits, static timing analysis (STA)
has  revealed  itself  as  the  only  feasible  method  ensuring  that  expected
performances are actually obtained.
  In addition, signal integrity (SI) issues due to crosstalk play a crucial role
in performance and reliability of these  systems, and must be taken into account
during the timing analysis.
  However, performance  achievement not  only lies in  fabrication technologies,
but also  in the way circuits  are designed.  Very high  performance designs are
obtained with semi or full-custom designs techniques.
  The HITAS platform provides advanced STA and SI solutions at transistor level.
It has been  built-up in order to allow engineers to  ensure complete timing and
SI coverage on their digital custom  designs, as well as IP-reuse through timing
abstraction.
  Furthermore,  hierarchy  handling  through  transparent  timing  views  allows
full-chip verification, with virtually no limit of capacity in design size.


%prep
%setup


%build
# Compile the local version of Flex.
 tar zxf ./distrib_extras/flex-2.5.4_patch.tar.gz
 localInstall=`pwd`
 pushd flex-2.5.4
 ./configure --prefix=${localInstall}
 make install
 popd
 PATH=${localInstall}/bin:$PATH
 export PATH

# Setting up the build layout.
# tas/yag uses an old fashioned Makefile.
 pushd distrib
 buildDir=`pwd`
 AVERTEC_OS="Linux"

 buildDirs="api_include api_lib bin lib include man/man3 share/doc"
 for dir in ${buildDirs}; do
   mkdir -p ${dir}
 done
 ln -s sources obj
 ln -s share/etc etc

 makefileEnv=""

 pushd obj
 make WITH_FLEXLM=NOFLEX               \
      ALLIANCE_TOP=${buildDir}         \
      AVERTEC_TOP=${buildDir}          \
      AVERTEC_OS=$AVERTEC_OS           \
      AVERTEC_LICENSE=AVERTEC_DUMMY    \
      AVT_LICENSE_SERVER=house         \
      AVT_LICENSE_FILE=27009@house     \
      AVT_COMPILATION_TYPE=distrib     \
      AVT_DISTRIB_DIR=${buildDir}      \
      PACKAGING_TOP=${localInstall}

 popd

# Build documentation.
 PATH=`pwd`/bin:$PATH
 pushd docxml2
%ifarch x86_64
# wrap_nolicense makes core dump under 64 bits.
# Uses the precompiled version of the doc (from RHEL6 32bits).
 tar jxf docxml2-compiled.tar.bz2
%else
 make AVERTEC_TOP=${buildDir}/share
%endif
 popd

 popd


%install
 rm -rf %{buildroot}
 mkdir -p %{buildroot}%{_sysconfdir}/profile.d
 mkdir -p %{buildroot}%{_bindir}
 mkdir -p %{buildroot}%{_datadir}/tasyag/etc
 mkdir -p %{buildroot}%{_mandir}/man3

 for conf in avt.slib avttools.dtb Xtas Xyagle trmodel.cfg; do
   cp distrib/share/etc/$conf %{buildroot}%{_datadir}/tasyag/etc
 done

 for tool in avt_shell avtman xtas xyagle ttvdiff ttvren; do
   cp distrib/bin/$tool %{buildroot}%{_bindir}
 done

 cp distrib/share/etc/avt_env.sh %{buildroot}%{_sysconfdir}/profile.d
 cp -r distrib/share/tcl %{buildroot}%{_datadir}/tasyag
 cp distrib/man/man3/* %{buildroot}%{_mandir}/man3
 mv distrib/docxml2/compiled/{docavertec.html,docpdf,dochtml} .
 mv distrib/share/tutorials .


%clean
 rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc docavertec.html dochtml docpdf tutorials LICENSE-UPMC-NC.rst
%dir %{_sysconfdir}/profile.d
%dir %{_bindir}
%dir %{_mandir}/man3
%{_sysconfdir}/profile.d/avt_env.sh
%{_bindir}/*
%config %{_datadir}/tasyag/etc/*
%{_datadir}/tasyag/tcl/*
%{_mandir}/man3/*



%changelog
* Tue Apr 17 2012 Jean-Paul Chaput <Jean-Paul DOT Chaput [AT] lip6 DOT fr> - 3.5p5.1
- Initial packaging.
