********************************************************************************
Symantec(TM) Client Security	                                      README.TXT
Copyright 1999 - 2005 Symantec Corporation. All rights reserved.      	May 2005
********************************************************************************

Please review this document in its entirety before you install or roll out 
Symantec Client Security, or call for technical support. It contains information 
that is not included in the Symantec Client Security documentation or the 
online Help.


Installation and uninstallation issues
--------------------------------------
*  Symantec Packager is unsupported
*  Symantec Client Security cannot be added to a software installation Group 
   Policy Object for Active Directory (R) deployment if the same version of 
   Symantec Client Security is already installed
*  Restart may be necessary for Symantec Client Security, Central Quarantine, 
   Symantec System Center, and Alert Managment System(2) installs
*  Deploying software by using the CD from computers that run legacy 
   versions of Symantec Client Firewall may require you to disable 
   the firewall
*  Antivirus server installations require a restart if you install
   the Alert Management Service(2) software
*  Symantec Client Firewall installation - default settings changes
*  Communication problem when you uninstall a Symantec AntiVirus server 
   that is on the same computer as the Symantec System Center
*  Potential uninstall issues in Trial Version


Migration issues
----------------
*  Updating to a new parent server during client migration by using Grc.dat
*  Unmanaged to managed client migration is unsupported
*  Version 7.6.1 migration fails with opened client or server user interfaces


Deployment with third-party products
------------------------------------
*  Deploying user-based installation packages with elevated privileges fails


Management-related issues
-------------------------
*  Back up the \pki directory on your primary server
*  Unregistered file extensions for non-hidden files are resolved with the 
   FixFileTypes tool
*  Rolling back server system dates disables servers 
*  Stopping Symantec processes causes instability
*  SAVRoam /nearest command requires administrator rights on Windows
*  Deleting locked and empty server groups
*  Group settings are applied to out-of-sync clients
*  Symantec System Center prompts restricted users for server group
   certificate
*  Changing the management mode of a client
*  Users are no longer allowed to modify scheduled LiveUpdates
*  Information about a user's logon domain is not available until restart
*  Setting for allowing clients to modify LiveUpdate schedules is locked
*  When your client email applications use a single Inbox file
*  Only locked settings are propagated to clients
*  Simple host name resolution required to manage Symantec AntiVirus 
   servers and clients
*  Dragging and dropping servers after changing Login Certificate 
   Settings results in loss of communication
*  Time out-of-sync error when you promote a server to primary server


Antivirus client and server issues
----------------------------------
*  Cookies are not scanned
*  Internet E-mail Auto-Protect port changes are ignored
*  Client users who schedule scans must be logged on for the scans to run
*  Improving client restart performance
*  Adding a file to the Quarantine and removing it from its original location
*  Scanning files by type is no longer available
*  Cleaning risks and side effects
*  Cleaning worms and Trojan horses 
*  Do not use third-party security risk scanners when Tamper Protection 
   is enabled
*  Setting Client Tracking options
*  Auto-Protect option to scan for security risks does not apply to computers
   that run earlier versions of Symantec AntiVirus


NetWare information
-------------------
*  Auto-Protect option to scan for security risks does not apply to computers 
   that run NetWare
*  Quick Scans not supported on NetWare
*  Configuring exceptions for security risks


Symantec Client Firewall Administrator issues
---------------------------------------------
*  OutOfMemory error
*  "ignore pRule digest values" option no longer supported without Location-aware
   pRules
*  Saving legacy policies before applying them

Firewall issues
---------------
*  Check Point VPN client triggers the Bla Worm Trojan Horse rule
*  Displaying firewall notification text


64-bit support
--------------
*  Antivirus client is the only feature that is supported
*  LiveUpdate is the only virus definitions files update method supported
*  Managing 64-bit clients with the Symantec System Center
*  Side effects repair limitations


Documentation issues
--------------------
*  Port Status and Port Status Reporting features were dropped
*  Reference guide Event Log entries chapter contains wrong information
*  Installation guide incorrectly states support for Itanium(R) II processors


Other issues
------------
*  XP SP2 Firewall Status Incorrectly Reported As Enabled After Reboot
*  Using System Restore on computers that run Windows XP
*  16-bit files used by Symantec AntiVirus components 
*  About security risks
*  About cookies
*  Security risk best practices 



--------------------------------------------------------
Installation and uninstallation issues
--------------------------------------------------------

Symantec Packager is unsupported
--------------------------------
The use of Symantec Packager and PMI files is unsupported.


Symantec Client Security cannot be added to a software installation 
Group Policy Object for Active Directory(R) deployment if the same 
version of Symantec Client Security is already installed
--------------------------------------------------------------------------------
You cannot create a Group Policy Object (GPO) package for software installation 
when the same version of the application is installed on the computer. Create 
the Symantec Client Security installation GPO before you install Symantec 
Client Security to the server.


Restart might be necessary for Symantec Client Security, Central 
Quarantine, and Symantec System Center installs
----------------------------------------------------------------
Symantec Client Security replaces system files and drivers that the operating
system might currently be using.  Microsoft(R) Installer (MSI) determines that a
reboot of the system is necessary and prompts the user with a request to reboot.

Depending on installation options, the system files might include:

C:\WINDOWS\system32\ 
   NavLogon.dll, S32EVNT1.DLL, SymNeti.dll, SymRedir.dll, atl70.dll,
   capicom.dll, cba.dll, comctl32.ocx, loc32vc0.dll, msgsys.dll
   msvci70.dll, msvcp70.dll, msvcr70.dll, nts.dll, and pds.dll
C:\WINDOWS\system32\drivers\ 
   SYMEVENT.SYS, SymIDSCo.sys, symdns.sys, symfw.sys, symids.sys,
   symndis.sys, symredrv.sys, and symtdi.sys

A restart might also be necessary for Central Quarantine because it also updates 
system files. A restart might also be necessary for the Symantec System Center 
since it requires the update of Microsoft Management Console plug-ins.
Antivirus management server installs also require a restart if you install
the Alert Management Service(2) software. 


Deploying software by using the CD from computers that run 
legacy versions of Symantec Client Firewall may require you to disable 
the firewall
----------------------------------------------------------------------
If you attempt to deploy the latest software by using the CD from 
computers that run legacy versions of Symantec Client Firewall, the
rulebases might not permit the network communications that are required
for remote deployment. To permit deployment, either create rules that 
permit the required network communications, or disable the firewall 
during deployment. The installation guide chapter about planning lists
the port numbers that you must open.


Antivirus server installations require a restart if you install
the Alert Management Service(2) software
---------------------------------------------------------------
If you select to install the Alert Management Service(2) software 
when you install an antivirus management server, you must restart
the computer before you can configure alerts.


Symantec Client Firewall installation - default settings changes
----------------------------------------------------------------
The Symantec Client Firewall installation now sets the custom Client 
Firewall level to Medium (previously High) and disables Privacy Control 
by default. The Client Firewall Medium setting still protects client 
computers from Trojan horses, intrusion attacks, and other potentially 
harmful network traffic by using improved Intrusion Prevention 
technology, Trojan rules, and General rules. Network traffic that is not 
specifically blocked is now allowed. When Privacy Control is disabled, 
sending Web browser information is permitted.

To install Symantec Client Firewall with its previous default custom 
Client Firewall level (High) and Privacy Control (Enabled) settings, you 
can delete CPOLICY.XML from the Symantec Client Security installation 
folder. When the custom Client Firewall level is set to High, network 
traffic that is not specifically permitted is blocked. You might find 
that this setting is too restrictive for the initial implementation of 
Symantec Client Firewall in your network. After you install Symantec 
Client Firewall, you can determine the level of security that is 
necessary for your client computers based on your corporate security 
policy and network infrastructure.

To delete CPOLICY.XML

1. Navigate to and double-click the Symantec Client Security folder.
2. In the right pane, locate and highlight CPOLICY.XML.
3. Right-click CPOLICY.XML, and then click Delete.
4. In the Confirm File Delete dialog box, click Yes.


Communication problem when you uninstall a Symantec AntiVirus server 
that is on the same computer as the Symantec System Center 
--------------------------------------------------------------------
If you uninstall a Symantec AntiVirus server that is on the same 
computer as the Symantec System Center, and then reinstall the server, it 
cannot communicate with the Symantec System Center. To work around this 
issue, you can uninstall both the Symantec AntiVirus server and the 
Symantec System Center, then install them again.

Potential uninstall issues in Trial Version
-------------------------------------------
Potential issues may exist in a managed environment when uninstalling 
the Trial version followed by an install and subsequent uninstall of 
the Non-Trial version. Refer to Knowledge Base articles for more information.

--------------------------------------------------------
Migration issues
--------------------------------------------------------


Updating to a new parent server during client migration by using Grc.dat
------------------------------------------------------------------------
If you migrate a client from a previous version to the current version 
and you are assigning the client to a new parent server, you must add 
the name of the new parent server to the RoamManagingParentLevel0 
entry in the Grc.dat file that you include. This entry is 
a comma-delimited list of available parent servers. If the name of 
the new server is not added to this list, the migrating client will not 
use the new server that you specified.


Unmanaged to managed client migration is unsupported
----------------------------------------------------
The migration of an unmanaged client to a managed client is not supported. 
To resolve this issue, when a managed client is installed over an unmanaged 
client, you can copy a Grc.dat file (which specifies that the client 
be managed by a specific parent server) into the appropriate directory.
The installation chapter in the installation guide describes how to perform
this conversion. 

If you do not want to use a GRC.dat file, the following procedure also 
resolves this issue:

1. Stop the RTVSCAN service.
2. Disable the two inbound NETBIOS rules in the Symantec Client Firewall.
3. Restart the RTVSCAN service.
4. Once the address cache has been populated, reenable the two inbound NETBIOS 
rules in Symantec Client Firewall.


Version 7.6.1 migration fails with opened client or server user interfaces
--------------------------------------------------------------------------
If you migrate version 7.6.1 servers or clients, and if the user 
interface is open, the migration fails. To migrate version 7.6.1 servers 
and clients to the latest version, close the user interfaces before you
begin migration.



--------------------------------------------------------
Deployment with third-party products
--------------------------------------------------------

Deploying user-based installation packages with elevated privileges fails
-------------------------------------------------------------------------
By setting the Active Directory group policy for Always Install With Elevated
Privileges, users without administrator rights can install Windows Installer
packages. If you install per machine, this setting permits installation to
succeed. If you install per user, this setting causes the installation to
fail.


--------------------------------------------------------
Management-related issues
--------------------------------------------------------

Back up the \pki directory on your primary server
-------------------------------------------------
If you have a server group that contains one server only, that server
is a primary server and manages all clients in the server group. 
If for some reason you have to reinstall server software on the
primary server, you will lose all communications with your clients. 
The reason is that you created a new server group root certificate
that the clients do not trust.

To mitigate this potential problem, always install a secondary server
in your server group so that you can unlock your server group. Further,
always back up the entire \pki subdirectory that is located in the 
directory that contains your server software. If you have the \pki
subdirectory available to restore after your reinstall server software, 
you can reestablish client communications. For detailed procedures,
contact your Symantec technical support representative.


Unregistered file extensions for non-hidden files are resolved with the 
FixFileTypes tool
-----------------------------------------------------------------------
Symantec Client Security includes file extensions for non-hidden files that are 
not registered on Windows operating systems. To mark these file types as 
hidden, on the Symantec Client Security CD, in the \Tools\FixFileTypes folder, 
run FixFileTypes.exe.


Rolling back server system dates disables servers 
-------------------------------------------------
If you roll back the system date on your primary server to a date that
precedes the server group root certificate creation date, you will not 
be able to use your server.


Stopping Symantec processes causes instability
----------------------------------------------
If you stop Symantec processes that run in the background, the computers
on which you stop the processes will become unstable. For a list of Symantec
processes, refer to the reference guide that is located on your
installation CD.


SAVRoam /nearest command requires administrator rights on Windows
-----------------------------------------------------------------
SAVRoam is typically configured to look for a new parent server after an
amount of time and after a computer restarts. However, the command-line
command SAVRoam /nearest lets you force SAVRoam to look for a new 
parent server immediately. To use this command, users must have
administrator rights on Windows computers.


Deleting locked and empty server groups
---------------------------------------
If you have a server group that contains a primary server only, and if
you uninstall the server, you cannot unlock the server group and delete
it. 

To delete the server group:
1. In the Symantec System Center console, click Tools > Discovery
   Service.
2. In the Discovery Services Properties dialog box, click Clear
   Cache Now.


Group settings are applied to out-of-sync clients
-------------------------------------------------
By default, clients that have system clocks set more than 24 hours plus or 
minus of the time set on the primary management server do not let 
administrators directly configure settings. For example, you cannot
right-click on an out-of-sync client in the Symantec System Center and 
view the client logs. Out-of-sync clients, however, do accept settings 
that administrators apply to groups.  For example, if you right-click a 
group in the Symantec System Center and change the Client Auto-Protect
setting, the out-of-sync client accepts the new setting. 

The reason that you cannot directly configure client settings is because
the system uses the Login certificate, which is valid for a specified
time only. You can change the times in the Symantec System Center with 
Configure login certificate settings at the group level. The reason 
that the out-of-sync clients accept group-level changes is because the 
system uses the server certificate, which is valid for five years.

For more information about certificates, refer to the Reference Guide
in the Docs folder on the installation CD.


Symantec System Center prompts restricted users for server group certificate
----------------------------------------------------------------------------
When you first unlock a server group, the Symantec System Center prompts
you to copy the server group root certificate to the Symantec System Center
directory structure. If you subsequently log on to the computer that runs the
Symantec System Center with administrator rights and unlock the server group, 
you are not prompted to copy the server group root certificate. If you 
subsequently log on to the same computer with low-level user rights, you are
always prompted to copy the server group root certificate.


Changing the management mode of a client 
----------------------------------------
In Chapter 2 of the administrator's guide, in the procedure for changing 
an unmanaged client to a managed client, steps 5 and 6 are no longer 
necessary. If the pki\roots folder on the client is empty, then the 
new parent server now automatically copies the server group root 
certificate and places it in the pki\roots folder on the client after 
you copy the Grc.dat file to the client and restart the client. 
If the pki\roots folder on the client contains its previous server 
group root certificate, you should delete it before you copy the new 
Grc.dat file to the client. 


Users are no longer allowed to modify scheduled LiveUpdates
-----------------------------------------------------------
The "Do not allow client to modify LiveUpdate schedule" option has been 
disabled on the Virus Definition Manager dialog box in the Symantec 
System Center. When you check the "Do not allow client to manually 
launch LiveUpdate" option or the "Schedule client for automatic updates 
using LiveUpdate" option, users are not allowed to modify any scheduled 
LiveUpdates that you configure. This automatic locking ensures that 
LiveUpdates that administrators schedule are always propagated to 
clients and cannot be modified by users.


Information about a user's logon domain is not available until restart  
----------------------------------------------------------------------
After an initial client software installation, the user's logon domain 
information does not appear in the Symantec System Center until the 
client computer is restarted. After a restart, this information is 
available in the Symantec System Center Symantec AntiVirus View, the 
Symantec Client Firewall View, the network audit results, the Event 
Log, the Risk History, and the Tamper History. In the Symantec 
AntiVirus user interface, it is available in the Event Log, the 
Risk History, and the Tamper History.


Setting for allowing clients to modify LiveUpdate schedules is locked
---------------------------------------------------------------------
The Virus Definition Manager window in the Symantec System Center 
contains the following setting, which is locked and dimmed:

o  Do not allow client to modify LiveUpdate schedule

When both of the following settings are disabled, the locked setting
is automatically unchecked and disabled:

o  Schedule client for automatic updates using LiveUpdate
o  Do not allow client to manually launch LiveUpdate

When one or both of these settings are checked and enabled, the locked
setting is automatically checked and enabled. If this setting was 
not locked, client users could create or modify schedules that conflict 
with the group policy and would not receive group-scheduled virus 
definitions updates.


When your client email applications use a single Inbox file 
-----------------------------------------------------------
If your clients use email applications that store all email in a 
single file, such as Outlook Express, Eudora, Mozilla, and Netscape, 
you might want to exclude the Inbox file from manual and scheduled 
scans. If Symantec AntiVirus catches a virus in the Inbox file 
during a manual or scheduled scan, and the action configured for 
the virus is Quarantine, Symantec AntiVirus quarantines the 
entire Inbox and users cannot access their email.

Although regularly excluding a file from scanning is not recommended
as a general practice, excluding the Inbox file from being scanned 
prevents it from being quarantined while still allowing a virus to 
be detected. If Symantec AntiVirus finds a virus when you open an 
email message rather than when you download the message or during a 
scan, it can safely quarantine or delete the message without causing 
a problem for the entire Inbox.


Only locked settings are propagated to clients
----------------------------------------------
To change settings on clients, you must lock the settings in the Symantec
System Center. If you change a setting for clients, and if that setting 
is not locked, the change does not occur on the clients. This feature 
also affects client installations by using the ClientRemote feature in 
the Symantec System Center. Only changed settings that are locked are 
configured on clients during installation. 


Simple host name resolution is required to manage Symantec AntiVirus 
servers and clients
--------------------------------------------------------------------
You must have simple host name resolution configured in your environment 
to manage Symantec AntiVirus servers and clients.  Fully qualified 
domain name resolution is not required.


Dragging and dropping servers after changing Login Certificate 
Settings results in loss of communication
--------------------------------------------------------------
In the Symantec System Center, when you increase the time interval 
set for the two options in the Login Certificate Settings dialog box 
to more than one day (24 hours) to account for time out-of-sync issues 
between servers and clients, and then drag and drop a server into a 
new server group, communication between the server and the Symantec 
System Center is lost.  This does not occur if your actual time 
discrepancy is 24 hours or less.


Time out-of-sync error when you promote a server to primary server
------------------------------------------------------------------
When you promote a server to primary in the Symantec System Center, 
you might get a login certificate time out-of-sync error. In most 
instances, you can work around this issue by clearing the cache, and 
then running a new Discovery. You can then promote the server to be
a primary server.



----------------------------------
Antivirus client and server issues
----------------------------------


Cookies are not scanned
-----------------------
Cookies are not scanned for viruses, threats, or security risks.


Internet E-mail Auto-Protect port changes are ignored
-----------------------------------------------------
The antivirus client Auto-Protect feature for Internet E-mail Advanced
Options lets you change the ports for POP3 and SMTP. The defaults 
for these ports are 110 and 25. The antivirus client ignores 
changes to these defaults. This issue applies to all email 
programs that use POP3 and SMTP, including Microsoft Outlook.

If you change these defaults with the antivirus client but your email
program uses the defaults, Auto-Protect still scans for risks in your
email traffic. If your email program does not use the defaults and
you change the Auto-Protect ports to match the ports used by your
email program, Auto-Protect does not scan for risks in your email 
traffic.


Client users who schedule scans must be logged on for the scans to run
----------------------------------------------------------------------
End users who schedule scans with the antivirus client user interface 
to run when they are not using their computers must be logged on to 
the computer or the scan does not run. Also, if a scheduled scan runs
and the user logs off, the scan stops running. The workaround 
is to have the users lock their computers without logging off. Scans
that are scheduled to run on clients in a server group are not
affected, and always run when scheduled.


Improving client restart performance
------------------------------------
By default, the antivirus client software performs an Auto-Generated
QuickScan when the client computer restarts. QuickScan includes an
enhancement that scans for security risks such as adware and spyware.
This enhancement increases the time required to restart client computers,
and is not configurable from the Symantec System Center.

To improve client startup performance:
1. In the Symantec AntiVirus window, in the left pane, expand 
   Startup Scans, and then click Auto-Generated QuickScan.
2. In the right pane, click Edit.
3. In the Scan dialog box, on the Quick Scan tab, click Options.
4. In the Scan Options dialog box, under Scan Enhancements, uncheck
   Scanning for traces of well-known viruses and Security Risks.
5. Click OK. 
6. In the Scan dialog box, click OK.


Adding a file to the Quarantine and removing it from its 
original location
--------------------------------------------------------
The option to "Remove file from original location" in the Add File 
to Quarantine dialog box does not work after you turn Auto-Protect 
off. When you use the Quarantine View icon to add a file to the 
Quarantine, and you uncheck the option to remove the file from its 
original location, the file is still removed. 


Scanning files by type is no longer available 
---------------------------------------------
Scanning files by type is no longer an option when you configure 
any scan. All types of files are scanned. Any previously configured 
scan that is migrated to the current version will also scan all 
file types. All information about scanning by selected file type 
in the administrator's guide is no longer applicable.


Cleaning risks and side effects
-------------------------------
When Symantec AntiVirus handles complicated risks and their side 
effects, at times it applies the first and second actions you 
configure to different portions of the risk. 

For example, when Symantec AntiVirus detects risks such as the 
Trojan.QQMess Trojan horse, if the first and second actions 
configured are Clean risk and Leave alone (log only), it might appear 
as if the first action failed on the detected files but 
was applied to some components of the risk. This occurs because 
if Symantec AntiVirus detects an infected file that it cannot 
clean, Symantec AntiVirus takes the second action and leaves the 
files alone. However, Symantec AntiVirus applies the first action 
to clean the side effects of the risk, and places these 
remediation targets in the Quarantine.

If the first and second configured actions are Clean risk and 
Delete risk, and Symantec AntiVirus cannot clean the files, 
Symantec AntiVirus applies the second action to the infected 
files and deletes them. Symantec AntiVirus applies the first 
action to the side effects of the risk, cleaning them and placing 
the remediation targets in the Quarantine.


Cleaning worms and Trojan horses 
--------------------------------
When the first configured action for non-macro viruses is Clean risk, 
the action that Symantec AntiVirus takes when it detects some worms 
and Trojan horses by a manual or scheduled scan is different from the 
action that Symantec AntiVirus takes when it detects the same risks by 
an Auto-Protect scan.

In rare cases, when a worm or Trojan horse cannot be cleaned from a 
file, but Auto-Protect determines that it is too dangerous to leave 
on the computer, Symantec AntiVirus deletes the infected files even 
when the first configured action for Auto-Protect is Clean risk and 
the second configured action for Auto-Protect is Leave alone (log 
only).  If a manual or scheduled scan has the same actions configured 
and detects the same uncleanable risk, the Clean action fails and the 
files are left alone (the configured second action).  To remove the 
risk from the computer, you must either change the configured action 
to Delete risk or Quarantine risk, or you must manually remove these files.


Do not use third-party security risk scanners when Tamper Protection 
is enabled
--------------------------------------------------------------------
The antivirus clients and servers have a new real-time feature called 
Tamper Protection that protects Symantec processes from unauthorized access
and tampering. If you run third-party security risk scanners that detect and
defend against unwanted adware and spyware, these scanners generally touch 
Symantec processes. Tamper Protection then generates tens and possibly 
hundreds of alerts and log entries. If you want to use third-party security 
risk scanners, disable Tamper Protection.


Setting Client Tracking options
-------------------------------
In the Symantec System Center, when you set Client Tracking options in the 
Server Tuning Options dialog box, the changes do not take effect until you 
restart the Rtvscan service on the server that you are configuring. 


Auto-Protect option to scan for security risks does not apply to computers 
that run earlier versions of Symantec AntiVirus 
--------------------------------------------------------------------------
When you configure Auto-Protect options in the Symantec System Center, the 
option to Scan for Security Risks does not apply to computers that run 
earlier versions of Symantec AntiVirus.



-------------------
NetWare information
-------------------


Auto-Protect option to scan for security risks does not apply to computers 
that run NetWare 
--------------------------------------------------------------------------
When you configure Server Auto-Protect options in the Symantec System Center, 
the option to Scan for Security Risks does not apply to computers that run 
NetWare.


Quick Scans not supported on NetWare
------------------------------------
Quick Scans are not supported on computers that run NetWare. 


Configuring exceptions for security risks 
-----------------------------------------
When you configure scan options in the Symantec System Center for 
Auto-Protect and other scans, if the parent server or primary server 
runs NetWare, the Available risks list in the Select risks dialog 
box is empty and you cannot configure exceptions.  To work 
around this issue, use a computer running Windows as your primary 
or parent server, or configure exceptions directly on the 
computers managed by the NetWare parent and primary servers.



---------------------------------------------
Symantec Client Firewall Administrator issues
---------------------------------------------

OutOfMemory error
-----------------
If you receive OutOfMemory errors, you might need to increase the maximum heap
memory for Java Virtual Machine. To increase the maximum heap memory, you
need to modify the -Xmx128m switch in the Shortcut target binary, where 128 is
the default memory size. You can increase the size in multiples of 16.

To increase the maximum heap memory:
1. Right-click the Symantec Client Firewall Administrator icon, and then click
   Properties.
2. On the Shortcut tab, in the target box, to the right of the text 
   ...javaw.exe", find and increase the value of -Xmx128m. 

   The entry should look something like the following:

   "C:\Program Files\Java\j2re1.4.1_05\bin\javaw.exe" -Xmx160m -DFre...

3. Click Apply, and then click OK.


"ignore pRule digest values" option no longer supported without Location-aware 
pRules
------------------------------------------------------------------------------
The "ignore pRule digest values" option is no longer supported without 
Location-aware pRules.  This implies that any pRule that has a required digest
 specified in the match criteria must match the digest value, or the rule for 
the application cannot be auto-created.  If the pRule does not have a required 
digest and matches another criterion, the user is notified with an alert, or the 
application is blocked or permitted based on the firewall level.  If an alert 
is shown, the user can choose to auto-create the rule for the application at 
this time.

To auto-create pRules that do not have a required digest defined in the match 
criteria or have the required digest specified in a separate match section, 
you can use the Location-aware pRules feature.  Configure a Location to 
Block selected on the pRules Settings tab, and then enable the "ignore pRule 
digest values" setting on the Client Settings tab.  Note that if the pRule 
has a required digest specified and it is not matched, the rule for the 
application cannot be auto-created.


Saving legacy policies before applying them
-------------------------------------------
Legacy policies must be saved by using the current version of Symantec Client 
Firewall Administrator before being applied to current Symantec Client Security 
clients.



--------------------------------------------------------
Firewall issues
--------------------------------------------------------


Check Point VPN client triggers the Bla Worm Trojan Horse rule
--------------------------------------------------------------
As part of its normal behavior, the Check Point VPN client triggers the Bla Worm 
Trojan Horse rule. If you run both the Check Point VPN client and the 
Symantec Client Firewall on the same computer, you need to do the following:

-  Create a custom policy file that allows Check Point VPN-1 SecuRemote to 
   run without interference. 
-  Include the new policy as part of the Symantec Client Firewall 
   deployment.

To include the new policy as part of the client firewall deployment:
1. Create a Program rule for the Check Point VPN client that includes the 
   Check Point VPN client executable file name and path on the computer that 
   runs Symantec Client Firewall. 

   If you cannot determine the Check Point VPN client executable file name and 
   location, you can remove the Bla Worm Trojan Horse rule; however, this 
   compromises security.  
2. Save the policy file as Cpolicy.xml.
3. Copy Cpolicy.xml to the folder on the client computer in which the Windows 
   Installer (.msi) package is located.


Displaying firewall notification text
--------------------------------------
Firewall notifications might not display all text if user appearance settings 
have been set to large or extra large fonts.



--------------------------------------------------------
64-bit support
--------------------------------------------------------

Antivirus client is the only feature that is supported
------------------------------------------------------
The antivirus client provides antivirus and security risk protection for 
clients and servers, and is the only feature supported for this release. All 
other components, products, and tools are not supported.


LiveUpdate is the only virus definitions files update method supported
----------------------------------------------------------------------  
LiveUpdate is the only virus definitions files update method supported. All 
other update methods, including the Virus Definition Transport Method, are 
not supported. 


Managing 64-bit clients with the Symantec System Center
-------------------------------------------------------
To manage 64-bit clients with the Symantec System Center, configure 
the client settings so that the clients do not receive automatic
virus definitions updates. The easiest way to configure the 
client settings is to configure a client group and place your 64-bit clients
in this group. You can rely on users to click LiveUpdate, or you can 
set a LiveUpdate schedule for the group by using the Symantec System Center.

To manage 64-bit clients with the Symantec System Center:
1. Create a client group.
2. Right-click the client group.
3. Uncheck Inherit settings from Server Group.
4. Right-click the client group, and then select All Tasks > Symantec
   AntiVirus > Virus Definition Manager.
5. In the Virus Definition Manager dialog box, uncheck Update virus
   definitions from parent server.
6  Do one or both of the following:
   o  Uncheck Do not allow client to manually launch LiveUpdate.
   o  Check Schedule client for automatic updates using
      LiveUpdate, click Schedule, specify a schedule, and then click OK.
7. Click OK.
   
   After you install 64-bit client software, drag the client computers
   into this client group. 


Side effects repair limitations
-------------------------------
64-bit antivirus clients support side effect repair only for 32-bit viruses 
and security risks that have infected the WOW64 portions of the 64-bit 
operating systems on AMD(R)/64 and EM64T hardware. 64-bit antivirus clients
do not currently support the repair of 64-bit viruses and security risks 
on the 64-bit native operating system facilities outside of WOW64 on AMD/64 
and EM64T hardware. 



--------------------------------------------------------
Documentation issues
--------------------------------------------------------

Port Status and Port Status Reporting features were dropped
-----------------------------------------------------------
The installation guide incorrectly lists Port Status and Port Status
Reporting as new Symantec Client Firewall features in the What's New
section. 


Reference guide Event Log entries chapter contains wrong information
--------------------------------------------------------------------
The Event Log entries chapter in the reference guide associates incorrect
event numbers with events.  Typically, above event number 40, the numbers
in the reference guide are one number low. 


Installation guide incorrectly states support for Itanium(R) II processors
--------------------------------------------------------------------------
The system requirements incorrectly state support for Itanium II processors,
which was dropped. This release supports Intel processors that support 
Intel Extended Memory 64 Technology (Intel EM64T), and AMD(R) 64-bit 
Opteron(TM) and Athlon(TM) processors.



--------------------------------------------------------
Other issues
--------------------------------------------------------

XP SP2 firewall status incorrectly reported as enabled after restart
--------------------------------------------------------------------
In a specific circumstance, the Windows Firewall might temporarily report itself
as enabled when it actually is not.  This occurs when the following installation
sequence has occurred:
1.  XP SP1 is installed.
2.  Symantec Client Security is installed and is configured to disable Windows
    Firewall (default).
3.  XP SP2 is installed.

In this case, the Windows Security Center correctly displays Windows
Firewall as disabled after a one minute wait.


Using System Restore on computers that run Windows XP
-----------------------------------------------------
The Symantec service Rtvscan.exe disables the System Restore Windows XP 
functionality. 

To use System Restore on computers that run Windows XP:
1. Disconnect the computers from your network.
2. Display the Administrate tool called Services.
3. In the Services window, stop Symantec AntiVirus.
4. Use System Restore to restore the computers to a known state.
5. Restart the computers.



16-bit files used by Symantec AntiVirus components 
-------------------------------------------------- 
The following 16-bit files are included with Symantec AntiVirus as part of 
the virus definitions files that are distributed with each installation and 
definitions update. Although the current version of Symantec AntiVirus 
is not supported on 16-bit operating systems, these 16-bit virus definitions 
files are provided for backwards compatibility with legacy versions of 
Symantec AntiVirus that might still be in use. 

Files that are associated with NetWare protection are stored on the computer 
on which the Symantec System Center is installed. These files are pushed to 
NetWare servers that might support 16-bit NetWare clients. These 16-bit files
are only executed on the target computers that receive these files; the files 
are not executed on the managing computer. 

Installations and updates on supported Windows platforms might include: 
Directories: 
 C:\Program Files\Common Files\Symantec Shared\VirusDefs\BinHub\ 
 C:\Program Files\Symantec\Quarantine\Server\Signatures\00027599\ 
Files: 
 ECBOOTIL.VXD 
 NAVENG.VXD 
 NAVENG16.DLL 
 NAVEX15.VXD 
 NAVEX16A.DLL 

16-bit files that are pushed from the Symantec System Center to supported NetWare 
platforms might include: 

Directory on the managing computer: 
 C:\Program Files\Symantec\Symantec System Center\Deployment\Server Rollout\ 
    SERVER\NETWARE\LOGIN\ 
Files: 
 CTL3D.DLL 
 DY_LOH.DLL 
 I2_LDVP.DLL 
 LDRTSC16.386 
 MSCOMSTF.DLL 
 MSDETSTF.DLL 
 MSINSSTF.DLL 
 MSSHLSTF.DLL 
 MSUILSTF.DLL 
 NAVAPI16.DLL 
 SETUP.EXE 
 VPCCC16.EXE 
 VPDNUI.EXE 
 VPDN_FTP.EXE 
 VPDOWN.EXE 
 VPREMOVE.EXE 
 WPUSHPOP.EXE


About security risks
--------------------
When possible, installers for security risks are detected 
and removed prior to them loading adware and spyware programs 
on a system. This is an effective approach when the installer 
application is solely used for delivery of security risks to a 
host machine. Nonetheless, more general purpose installers may 
very well be used in conjunction with spyware or adware and 
cannot be blocked given the broad range of application types 
they serve. When it is not possible to block the security risk 
installer, the spyware or adware application is allowed to 
load into the system for several reasons. 

Firstly, interruption of a partially installed spyware program 
may put the host in an unstable state, leaving the user with 
error messages or residual files and folders left on the machine. 
If the installation is completed prior to removal, all aspects 
of the security risk can be analyzed and properly removed such 
that the host is left in a safe, predictable state. 

Secondly, spyware and adware programs behave in a fashion that 
can be very similar to a normal application as opposed to the 
more obvious, unusual behavior that malware typically exhibits. 
Thus, in order to ensure accurate detection, the program is allowed 
to load before it is fully identified and subsequently removed or 
quarantined. Lastly, security risks are potentially desirable 
programs and how acceptable they are is determined by the preferences
 of the user or administrator. Given this, ensuring such applications 
are not automatically blocked is essential.

Despite the fact that a security risk program may be momentarily 
loaded on a system, little can be accomplished by such a program 
given the additional security measures in place on a properly 
protected host. For example, customers using SCS/NIS benefit 
from the protection of robust firewalls that will block any 
attempts for the application to phone home or otherwise transmit 
data without the user's consent. Thus, in the brief time between 
the security risk being loaded and before it's removal, the risk 
of any unauthorized action being taken is extremely small.

We believe this approach to similar to that of our major 
competitors and have not seen evidence yet of an approach 
that is effective at achieving accurate detection and removal 
without full analysis of a security risk program such as adware 
or spyware.


About cookies
-------------
Cookies are a widely used technology for maintaining information 
during and across Web site sessions. There are a few major forms 
of cookies, each with a separate intent and usage. The first major 
difference among cookies is whether or not they remain on your 
system after you visit a Web site. If they remain (as part of a 
small text file), they are called �persistent� cookies which can be 
used by the Web site to help tailor your experience the next time 
you visit based on your previous actions. If they do not, they 
are called �session� or temporary cookies and are deleted after 
you close your browser.
 
The second major distinction among cookies is that of first party 
versus third party cookies. First party cookies are presented by 
the Web site you are visiting and are only used by that Web site. 
Third party or tracking cookies are presented and used by one 
or many Web site to track basic online behavior within and across 
Web sites, typically for online marketing purposes. While there 
is a general acceptance of most types of cookies, third party 
cookies have been considered objectionable by some since they 
have the potential to disclose information such as Web browsing 
behavior or more personal data to marketers.
 
While there have been privacy concerns since the introduction 
of cookies in 1996, early concerns were eventually alleviated 
as understanding of the technology grew and Web browsers gave 
users the ability to clearly state their preference for handling 
cookies. Specifically, popular Web browsers today such as Internet 
Explorer allow users to set their privacy preferences, including 
detailed handling of third party cookies, and the default settings 
prevent practices such as personally identifiable information 
being tracked without a user�s explicit consent. 
 
Alongside the ready availability of features to control use of the 
third party cookies, forthcoming legislation has made that cookies 
will not be included within the scope of regulated software or features. 
The leading Federal bill, the SPY ACT or H.R. 29, was recently amended 
to exclude third party cookies from the scope of regulation so as to 
not unfairly restrict online commerce.
 
Given the ease at which customers can control cookies within today�s 
Web browsers as well as the widespread nature of more serious security 
risks such as spyware, Symantec does not detect the presence of cookies 
on a system today in order to better focus customers on the most 
pressing security issues.
 

Security risk best practices 
----------------------------
o  If a customer does not take an aggressive stance against Spyware 
   or Adware removal Symantec recommends that Real-Time scanning be 
   turned off for that category.
o  Removing spyware/adware may require a process to be terminated 
   (for instance a Web browser), in some cases the system must be 
   rebooted to completely clean it.
o  Administrator can allow end-user to choose when to terminate/reboot.
o  If end-user delays this action, real-time protection will continue 
   to find the security risk.
o  This acts as an aggressive reminder.



*************************************************************************

If the software that you have licensed is Symantec Client Security, this 
software uses the Java(TM) 2 Runtime Environment (JRE2) Standard Edition, 
Version 1.4.2_x (c) Sun Microsystems, Inc. 1994-2005.

********************************************************************************
END OF FILE
********************************************************************************
