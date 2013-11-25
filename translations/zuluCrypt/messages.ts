<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0">
<context>
    <name>CreateVolumeDialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/createvolumedialog.ui" line="17"/>
        <source>warning!!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolumedialog.ui" line="29"/>
        <source>yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolumedialog.ui" line="42"/>
        <source>no</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolumedialog.ui" line="55"/>
        <source>This operation will lead to permanent destrunction of all present data in /dev/sdc1. Are you sure you want to continue?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolumedialog.cpp" line="41"/>
        <source>This operation will lead to permanent destrunction of all present data in &quot;%1&quot;.

Are you sure you want to continue?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolumedialog.cpp" line="69"/>
        <source>It is advised to create encrypted containers over random data to prevent information leakage.

Do you want to write random data to &quot;%1&quot; first before creating an encrypted container in it?

You can stop the random data writing process anytime you want if it takes too long and you can no longer wait.

</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>CryptTask</name>
    <message>
        <location filename="../../zuluCrypt-gui/crypttask.cpp" line="83"/>
        <source>calculating md5sum</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/crypttask.cpp" line="133"/>
        <source>creating encrypted container file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/crypttask.cpp" line="210"/>
        <source>copying data to the container file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/crypttask.cpp" line="270"/>
        <source>copying data from the container file</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>DialogMsg</name>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="17"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="29"/>
        <source>&amp;ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="42"/>
        <source>&amp;yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="55"/>
        <source>&amp;no</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="68"/>
        <source>text</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="90"/>
        <source>type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="109"/>
        <source>cipher</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="125"/>
        <source>key size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="141"/>
        <source>device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="160"/>
        <source>loop</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="179"/>
        <source>offset</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="195"/>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="243"/>
        <source>size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="211"/>
        <source>mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="227"/>
        <source>fs</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="259"/>
        <source>used</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="275"/>
        <source>unused</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="291"/>
        <source>used %</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="307"/>
        <source>active slots</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="85"/>
        <source>By default,volumes are mounted with &quot;noexec&quot; option and hence its not possible to run 
programs or scripts from them.To do so,create a group called &quot;zulumount-exec&quot; and add yourself to the group and volumes will start to be mounted with &quot;exec&quot; option and ability to runprograms or scripts from the volume will be enabled.

&quot;system volumes&quot; are volumes that either udev has identify them as such if udev is enabled or have an entry in &quot;/etc/fstab&quot;,&quot;/etc/crypttab&quot; or &quot;/etc/zuluCrypt-system&quot;.

If you prefer for a device not to be considered a system device,start the tool

from root account and then go to &quot;menu-&gt;options-&gt;manage non system partitions&quot;

and add the device to the list and the device will stop being considered as &quot;system&quot;.

Alternatively,you can add yourself to group &quot;zulucrypt&quot; and all restrictions will be gone</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="99"/>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="115"/>
        <source>INFORMATION</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="107"/>
        <source>insufficient privilege to access a system device,
only root user or members of group zulucrypt can do that</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="109"/>
        <source>insufficient privilege to access a system device in read/write mode,
only root user or members of group zulucrypt-write can do that</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="112"/>
        <source>you do not seem to have proper permissions to access the encrypted file in %1 mode,check file permissions and try again</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="272"/>
        <source>do not show this dialog again</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LxQt::Wallet::changePassWordDialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.cpp" line="91"/>
        <source>Create a new wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.cpp" line="93"/>
        <source>Create</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.cpp" line="118"/>
        <source>Passwords do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.cpp" line="157"/>
        <source>New passwords do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.cpp" line="221"/>
        <source>Wallet password could not be changed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.cpp" line="230"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LxQt::Wallet::password_dialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.cpp" line="89"/>
        <source>Yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.cpp" line="90"/>
        <source>No</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.cpp" line="99"/>
        <source>wallet could not be opened with the presented key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.cpp" line="156"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>PasswordDialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="26"/>
        <source>open encrypted volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="41"/>
        <source>&amp;open</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="60"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="79"/>
        <source>open the volume in &amp;read only mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="95"/>
        <source>select mount point path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="114"/>
        <source>open volume path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="164"/>
        <source>&amp;key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="180"/>
        <source>&amp;plugin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="193"/>
        <source>key &amp;from a key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="210"/>
        <source>open key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="249"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="268"/>
        <source>mount name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password.ui" line="287"/>
        <source>volume path</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="76"/>
        <source>
options:
	-d   path to where a volume to be auto unlocked/mounted is located
	-m   tool to use to open a default file manager(default tool is xdg-open)
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="82"/>
        <source>	-e   start the application without showing the GUI
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="93"/>
        <source>if the option is checked,a primary private mount point will be created in &quot;/run/media/private/$USER/&quot;
and a secondary publicly accessible &quot;mirror&quot; mount point will be created in &quot;/run/media/public/&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="246"/>
        <source>setting this option will cause the volume to open in read only mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="259"/>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="270"/>
        <source>info</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>changePassWordDialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="14"/>
        <source>change wallet&apos;s password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="42"/>
        <source>Change</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="55"/>
        <source>Ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="107"/>
        <source>&quot;qCheckGMail&quot; want to change &quot;wallet&quot; password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="126"/>
        <source>Enter current password below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="145"/>
        <source>Enter new password below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="164"/>
        <source>Re enter new password below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="183"/>
        <source>Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="196"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;An application &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%1&lt;/span&gt;&apos; has made a request for a password of its wallet &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%2&lt;/span&gt;&apos; to be changed&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;br /&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/changepassworddialog.ui" line="214"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Enter password information below to create a new wallet &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%1&lt;/span&gt;&apos; for application &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%2&lt;/span&gt;&apos;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>createfile</name>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="17"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="129"/>
        <source>create a container file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="39"/>
        <source>file name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="68"/>
        <source>file path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="87"/>
        <source>file size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="129"/>
        <source>open a folder dialog box</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="145"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="164"/>
        <source>% complete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="183"/>
        <source>c&amp;reate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="206"/>
        <source>KB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="211"/>
        <source>MB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.ui" line="216"/>
        <source>GB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="127"/>
        <source>1/2 create a container file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="143"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="146"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="149"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="157"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="163"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="166"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="184"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="143"/>
        <source>file name field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="146"/>
        <source>file path field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="149"/>
        <source>file size field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="157"/>
        <source>Illegal character in the file size field.Only digits are allowed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="163"/>
        <source>file with the same name and at the destination folder already exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="166"/>
        <source>you dont seem to have writing access to the destination folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="184"/>
        <source>container file must be bigger than 3MB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="213"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="216"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="213"/>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="216"/>
        <source>could not open cryptographic back end to generate random data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="228"/>
        <source>terminating file creation process</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="229"/>
        <source>are you sure you want to stop file creation process?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="249"/>
        <source>2/2 write random data to a container file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createfile.cpp" line="254"/>
        <source>Select Path to where the file will be created</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>createkeyfile</name>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="17"/>
        <source>create a key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="39"/>
        <source>keyfile name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="58"/>
        <source>path to a folder to create a key in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="71"/>
        <source>open a folder a key file will be created in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="87"/>
        <source>c&amp;reate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="106"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="125"/>
        <source>keyfile path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.ui" line="144"/>
        <source>rng</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="132"/>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="135"/>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="138"/>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="141"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="132"/>
        <source>the key name field is empth</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="135"/>
        <source>folder path to where the key will be created is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="138"/>
        <source>file with the same name and at the destination folder already exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="141"/>
        <source>you dont seem to have writing access to the destination folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="159"/>
        <source>WARNING!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="159"/>
        <source>process interrupted,key not fully generated</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="161"/>
        <source>SUCCESS!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="161"/>
        <source>key file successfully created</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createkeyfile.cpp" line="168"/>
        <source>Select a folder to create a key file in</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>createvolume</name>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="17"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="89"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="325"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="338"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="352"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="364"/>
        <source>create a new volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="39"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="175"/>
        <source>path to device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="58"/>
        <source>c&amp;reate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="77"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="105"/>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="157"/>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="300"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="322"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="350"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="118"/>
        <source>key from a keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="141"/>
        <source>open a key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="186"/>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="319"/>
        <source>repeat key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="259"/>
        <source>volume type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="361"/>
        <source>hidden key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="374"/>
        <source>hidden key from keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="387"/>
        <source>volume size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="417"/>
        <source>MB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="422"/>
        <source>KB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="427"/>
        <source>GB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="442"/>
        <source>rng</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.ui" line="458"/>
        <source>file system</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="73"/>
        <source>normal truecrypt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="74"/>
        <source>normal+hidden truecrypt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="84"/>
        <source>passphrase quality: 0/100</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="86"/>
        <source>passphrase quality: %1/100</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="101"/>
        <source>this tool expects to find file system creation tools at &quot;%1/&quot; </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="102"/>
        <source>and it can not find them.
It is therefore not possible to create volumes using this tool.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="103"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="376"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="384"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="391"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="396"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="400"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="409"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="415"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="504"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="505"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="507"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="508"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="509"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="510"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="511"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="512"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="513"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="514"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="515"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="516"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="517"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="518"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="519"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="520"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="521"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="522"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="523"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="524"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="525"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="526"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="527"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="528"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="529"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="180"/>
        <source>path to file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="228"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="236"/>
        <source>key file path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="335"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="362"/>
        <source>keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="376"/>
        <source>volume path field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="384"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="400"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="409"/>
        <source>atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="391"/>
        <source>illegal character detected in the hidden volume size field</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="396"/>
        <source>hidden passphrases do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="415"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="517"/>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="525"/>
        <source>passphrases do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="497"/>
        <source>volume created successfully</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="499"/>
        <source>

creating a backup of the luks header is strongly advised.
Please read documentation on why this is important.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="502"/>
        <source>SUCCESS!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="504"/>
        <source>presented file system is not supported,see documentation for more information</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="505"/>
        <source>insufficient privilege to open a system device in read/write mode,
only root user or members of group zulucrypt-system can do that</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="507"/>
        <source>could not create an encrypted volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="508"/>
        <source>could not open volume for writing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="509"/>
        <source>there seem to be an opened mapper associated with the device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="510"/>
        <source>can not create a volume on a mounted device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="511"/>
        <source>container file must be bigger than 3MB</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="512"/>
        <source>%1 not found</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="513"/>
        <source>insufficient memory to hold your response</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="514"/>
        <source>operation terminated per user request</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="515"/>
        <source>could not get passphrase in silent mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="516"/>
        <source>insufficient memory to hold the passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="518"/>
        <source>invalid path to key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="519"/>
        <source>could not get a key from a key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="520"/>
        <source>couldnt get enought memory to hold the key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="521"/>
        <source>could not get a key from a socket</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="522"/>
        <source>one or more required argument(s) for this operation is missing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="523"/>
        <source>can not get passphrase in silent mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="524"/>
        <source>insufficient memory to hold passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="526"/>
        <source>failed to create a volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="527"/>
        <source>wrong argument detected for tcrypt volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="528"/>
        <source>could not find any partition with the presented UUID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/createvolume.cpp" line="529"/>
        <source>unrecognized ERROR! with status number %1 encountered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>cryptfiles</name>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="39"/>
        <source>destination path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="74"/>
        <source>c&amp;reate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="93"/>
        <source>source path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="147"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="197"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="86"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="160"/>
        <source>key from a key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="226"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="87"/>
        <source>repeat key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="245"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.ui" line="271"/>
        <source>% complete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="79"/>
        <source>enter a key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="98"/>
        <source>enter a path to a keyfile location</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="99"/>
        <source>keyfile path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="143"/>
        <source>create encrypted version of a file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="152"/>
        <source>create decrypted version of an encrypted file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="226"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="231"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="234"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="242"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="246"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="249"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="259"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="363"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="365"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="367"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="370"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="372"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="376"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="380"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="382"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="384"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="226"/>
        <source>path to source field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="231"/>
        <source>invalid path to source file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="234"/>
        <source>destination path already taken</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="242"/>
        <source>first key field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="246"/>
        <source>second key field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="249"/>
        <source>keys do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="259"/>
        <source>invalid path to key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="302"/>
        <source>Select Path to put destination file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="328"/>
        <source>select a file you want to encrypt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="330"/>
        <source>zuluCrypt encrypted files ( *.zc ) ;; All Files ( * )</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="331"/>
        <source>select a file you want to decrypt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="339"/>
        <source>select a keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="358"/>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="361"/>
        <source>SUCCESS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="358"/>
        <source>encrypted file created successfully</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="361"/>
        <source>decrypted file created successfully</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="363"/>
        <source>could not open keyfile for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="380"/>
        <source>could not open reading encryption routines</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="382"/>
        <source>could not open writing encryption routines</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="384"/>
        <source>failed to close encryption routine</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="365"/>
        <source>could not open encryption routines</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="367"/>
        <source>file or folder already exist at destination address</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="370"/>
        <source>insufficient privilege to create destination file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="372"/>
        <source>presented key did not match the encryption key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="374"/>
        <source>INFO!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="374"/>
        <source>operation terminated per user request</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="376"/>
        <source>insufficient privilege to open source file for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="378"/>
        <source>WARNING</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptfiles.cpp" line="378"/>
        <source>decrypted file created successfully but md5 checksum failed,file maybe corrupted</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>cryptoinfo</name>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="17"/>
        <source>crypo info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="29"/>
        <source>ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="42"/>
        <source>default options for plain volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="58"/>
        <source>cipher: aes-cbc-essiv:sha256</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="74"/>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="122"/>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="280"/>
        <source>keysize: 256 bits</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="90"/>
        <source>password hash: ripemd160</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="106"/>
        <source>default creation options for luks volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="151"/>
        <source>luks header hashing: sha1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="180"/>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="264"/>
        <source>cipher: aes-xts-plain64</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="248"/>
        <source>default creation options for truecrypt volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/cryptoinfo.ui" line="296"/>
        <source>hash: ripemd160</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>erasedevice</name>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.ui" line="17"/>
        <source>erase data on the device by writing random data over them</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.ui" line="29"/>
        <source>path to device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.ui" line="74"/>
        <source>% completed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.ui" line="106"/>
        <source>&amp;start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.ui" line="122"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="44"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="84"/>
        <source>write random data over existing data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="63"/>
        <source>The next dialog will write random data to a device leading to permanent loss of all contents on the device.

Are you sure you want to continue? </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="67"/>
        <source>WARNING</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="92"/>
        <source>SUCCESS!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="92"/>
        <source>data on the device successfully erased</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="93"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="94"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="95"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="97"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="98"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="99"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="101"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="102"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="103"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="104"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="105"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="106"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="107"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="126"/>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="131"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="93"/>
        <source>could not create mapper</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="94"/>
        <source>could not resolve device path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="95"/>
        <source>random data successfully written</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="97"/>
        <source>operation terminated per user choicer</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="98"/>
        <source>can not write on a device with opened mapper</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="99"/>
        <source>policy prevents non root user opening mapper on system partition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="101"/>
        <source>device path is invalid</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="102"/>
        <source>passphrase file does not exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="103"/>
        <source>could not get enought memory to hold the key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="104"/>
        <source>insufficient privilege to open key file for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="105"/>
        <source>this device appear to already be in use</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="106"/>
        <source>can not open a mapper on a mounted device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="107"/>
        <source>could not write to the device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="126"/>
        <source>device path field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="131"/>
        <source>invalid path to device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="134"/>
        <source>Are you really sure you want to write random data to &quot;%1&quot; effectively destroying all contents in it?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="137"/>
        <source>WARNING!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="152"/>
        <source>writing random data over existing data</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="197"/>
        <source>enter path to volume to be erased</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/erasedevice.cpp" line="206"/>
        <source>select a non system partition to erase its contents</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>luksaddkey</name>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="17"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="98"/>
        <source>add a key to a luks volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="32"/>
        <source>volume path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="51"/>
        <source>open file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="67"/>
        <source>open partition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="98"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="124"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="236"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="281"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="189"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="212"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="111"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="249"/>
        <source>key from a key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="143"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="262"/>
        <source>open keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="173"/>
        <source>&amp;add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="192"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="320"/>
        <source>reenter key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="340"/>
        <source>key already in the encrypted volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.ui" line="356"/>
        <source> key to be added to the encrypted volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="93"/>
        <source>passphrase quality: 0/100</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="95"/>
        <source>passphrase quality: %1/100</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="149"/>
        <source>existing key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="158"/>
        <source>new key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="167"/>
        <source>encrypted volume path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="186"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="209"/>
        <source>enter a key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="197"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="222"/>
        <source>enter a path to a keyfile location</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="200"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="225"/>
        <source>keyfile path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="243"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="248"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="254"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="258"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="339"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="340"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="341"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="342"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="343"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="344"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="345"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="346"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="347"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="348"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="349"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="350"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="351"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="352"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="353"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="354"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="355"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="356"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="243"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="248"/>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="254"/>
        <source>atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="258"/>
        <source>keys do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="317"/>
        <source>key added successfully.
%1 / %2 slots are now in use</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="315"/>
        <source>key added successfully.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="321"/>
        <source>SUCCESS!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="339"/>
        <source>presented key does not match any key in the volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="340"/>
        <source>could not open luks volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="341"/>
        <source>volume is not a luks volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="342"/>
        <source>insufficient privilege to add a key to a system device,
only root user or members of group &quot;zulucrypt&quot; can do that
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="343"/>
        <source>could not open volume in write mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="344"/>
        <source>all key slots are occupied, can not add any more keys</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="345"/>
        <source>can not get passphrase in silent mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="346"/>
        <source>insufficient memory to hold passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="347"/>
        <source>new passphrases do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="348"/>
        <source>one or more required argument(s) for this operation is missing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="349"/>
        <source>one or both keyfile(s) does not exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="350"/>
        <source>insufficient privilege to open key file for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="351"/>
        <source>couldnt get enought memory to hold the key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="352"/>
        <source>could not get a key from a socket</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="353"/>
        <source>could not get elevated privilege,check binary permissions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="354"/>
        <source>can not find a partition that match presented UUID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="355"/>
        <source>device is not a luks device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksaddkey.cpp" line="356"/>
        <source>unrecognized ERROR! with status number %1 encountered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>luksdeletekey</name>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="17"/>
        <source>remove a key from a volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="42"/>
        <source>key </source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="73"/>
        <source>existing key in the volume to delete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="86"/>
        <source>existing key from a key file to delete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="100"/>
        <source>open a keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="119"/>
        <source>&amp;delete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="138"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="167"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="275"/>
        <source>volume path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="186"/>
        <source>open an encrypted file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.ui" line="205"/>
        <source>open an encrypted partition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="74"/>
        <source>enter a key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="75"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="108"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="85"/>
        <source>enter a path to a keyfile location</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="86"/>
        <source>keyfile path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="95"/>
        <source>key file with a passphrase to delete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="178"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="224"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="247"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="248"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="249"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="251"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="252"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="253"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="254"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="255"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="256"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="257"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="258"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="259"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="260"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="261"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="262"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="224"/>
        <source>atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="178"/>
        <source>volume is not a luks volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="181"/>
        <source>There is only one last key in the volume.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="182"/>
        <source>
Deleting it will make the volume unopenable and lost forever.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="183"/>
        <source>
Are you sure you want to delete this key?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="185"/>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="190"/>
        <source>WARNING</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="189"/>
        <source>are you sure you want to delete a key from this volume?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="244"/>
        <source>key removed successfully.
%1 / %2 slots are now in use</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="245"/>
        <source>SUCCESS!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="247"/>
        <source>there is no key in the volume that match the presented key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="248"/>
        <source>could not open the volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="249"/>
        <source>insufficient privilege to open a system device,only root user or members of group zulucrypt-system can do that</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="251"/>
        <source>could not open the volume in write mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="252"/>
        <source>insufficient memory to hold your response</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="253"/>
        <source>operation terminated per user request</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="254"/>
        <source>can not get passphrase in silent mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="255"/>
        <source>insufficient memory to hold passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="256"/>
        <source>one or more required argument(s) for this operation is missing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="257"/>
        <source>keyfile does not exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="258"/>
        <source>could not get enough memory to open the key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="259"/>
        <source>insufficient privilege to open key file for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="260"/>
        <source>could not get a key from a socket</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="261"/>
        <source>can not find a partition that match presented UUID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/luksdeletekey.cpp" line="262"/>
        <source>unrecognized ERROR! with status number %1 encountered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>manageSystemVolumes</name>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.ui" line="17"/>
        <source>manage system volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.ui" line="32"/>
        <source>&amp;done</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.ui" line="51"/>
        <source>add fi&amp;le</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.ui" line="70"/>
        <source>add dev&amp;ice</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.ui" line="108"/>
        <source>path to system volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="112"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="112"/>
        <source>could not open &quot;%1&quot; for writing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="144"/>
        <source>remove selected entry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="147"/>
        <source>cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="162"/>
        <source>are you sure you want to remove 
&quot;%1&quot;
 from the list?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="166"/>
        <source>WARNING</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managesystemvolumes.cpp" line="195"/>
        <source>select path to system volume</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>managedevicenames</name>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="17"/>
        <source>manage favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="29"/>
        <source>&amp;add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="64"/>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="89"/>
        <source>device address</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="72"/>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="108"/>
        <source>mount point path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="147"/>
        <source>open partition dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="163"/>
        <source>open file dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.ui" line="179"/>
        <source>&amp;done</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="139"/>
        <source>remove selected entry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="142"/>
        <source>cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="186"/>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="189"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="186"/>
        <source>device address field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="189"/>
        <source>mount point path field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/managedevicenames.cpp" line="204"/>
        <source>path to an encrypted file</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>manageluksheader</name>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.ui" line="17"/>
        <source>backup luks header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.ui" line="39"/>
        <source>backup name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.ui" line="74"/>
        <source>c&amp;reate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.ui" line="93"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.ui" line="112"/>
        <source>device path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="89"/>
        <source>restore luks header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="91"/>
        <source>&amp;restore</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="98"/>
        <source>back up luks header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="100"/>
        <source>&amp;backup</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="150"/>
        <source>select a file with a luks backup header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="155"/>
        <source>select a folder to store the header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="223"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="305"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="306"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="307"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="309"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="310"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="311"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="312"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="313"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="314"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="315"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="316"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="317"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="318"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="319"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="320"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="321"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="322"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="323"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="223"/>
        <source>atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="240"/>
        <source>Are you sure you want to replace a header on device &quot;%1&quot; with a backup copy at &quot;%2&quot;?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="242"/>
        <source>WARNING!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="278"/>
        <source>select luks container you want to backup its header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="291"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="293"/>
        <source>SUCCESS</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="291"/>
        <source>header saved successfully.
if possible,store it securely.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="293"/>
        <source>header restored successfully</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="305"/>
        <source>presented device is not a LUKS device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="306"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="307"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="310"/>
        <source>failed to perform requested operation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="308"/>
        <source>INFO!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="308"/>
        <source>operation terminater per user request</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="309"/>
        <source>path to be used to create a back up file is occupied</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="311"/>
        <source>insufficient privilege to open backup header file for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="312"/>
        <source>invalid path to back up header file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="313"/>
        <source>insufficient privilege to create a backup header in a destination folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="314"/>
        <source>invalid path to device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="315"/>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="316"/>
        <source>argument for path to a backup  header file is missing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="317"/>
        <source>only root user and &quot;zulucrypt&quot; members can restore and back up luks headers on system devices</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="318"/>
        <source>insufficient privilege to open device for writing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="319"/>
        <source>could not resolve path to device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="320"/>
        <source>backup file does not appear to contain luks header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="321"/>
        <source>insufficient privilege to open device for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="322"/>
        <source>device is not a luks device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/manageluksheader.cpp" line="323"/>
        <source>unrecognized ERROR! with status number %1 encountered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>oneinstance</name>
    <message>
        <location filename="../../zuluMount-gui/oneinstance.cpp" line="43"/>
        <source>There seem to be another instance running,exiting this one</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/oneinstance.cpp" line="45"/>
        <source>Previous instance seem to have crashes,trying to clean up before starting</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>openvolume</name>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="17"/>
        <source>select a partition to open</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="142"/>
        <source>partition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="150"/>
        <source>size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="158"/>
        <source>label</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="166"/>
        <source>type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="174"/>
        <source>uuid</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="35"/>
        <source>use uuid</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="48"/>
        <source>&amp;help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="64"/>
        <source>use &amp;uuid</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.ui" line="80"/>
        <source>&amp;cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="107"/>
        <source>a list of all partitions on this system are displayed here.
Double click an entry to use it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="110"/>
        <source>you are not root user and hence only non system partition are displayed on this list.
Please read documentation for more information on system/non system partitions.
Double click an entry to use it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="113"/>
        <source>you are a root user and all partitions are displayed.
Double click an entry to use it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="116"/>
        <source>info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="151"/>
        <source>select a partition to create an encrypted volume in</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="157"/>
        <source>select an encrypted partition to open</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="230"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/openvolume.cpp" line="230"/>
        <source>only cryto_LUKS volumes can be selected</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>passwordDialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="152"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="366"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="431"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="152"/>
        <source>could not find any plugin installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="161"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="179"/>
        <source>cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="251"/>
        <source>plugin path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="253"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="264"/>
        <source>plugin name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="260"/>
        <source>choose a module from the file system</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="261"/>
        <source>enter a module name to use to get passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="268"/>
        <source>select a key module</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="276"/>
        <source>enter a key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="280"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="290"/>
        <source>choose a key file from the file system</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="291"/>
        <source>enter a path to a keyfile location</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="295"/>
        <source>keyfile path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="308"/>
        <source>Select a keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="310"/>
        <source>Select a key module</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="327"/>
        <source>Select Path to mount point folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="344"/>
        <source>Select encrypted volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="425"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="451"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="464"/>
        <source>atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="431"/>
        <source>&quot;/&quot; character is not allowed in mount name field</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="624"/>
        <source>insufficient privilege to open device in read write mode or device does not exist</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="366"/>
        <source>the volume does not appear to have an entry in the wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="571"/>
        <source>warning</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="571"/>
        <source>could not open mount point because &quot;%1&quot; tool does not appear to be working correctly</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="425"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="451"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="464"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="588"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="619"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="620"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="621"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="622"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="623"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="624"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="625"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="626"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="627"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="628"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="629"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="630"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="631"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="632"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="633"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="634"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="635"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="636"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="637"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="638"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="639"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="640"/>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="642"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="588"/>
        <source>An error has occured and the volume could not be opened</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="619"/>
        <source>failed to mount ntfs file system using ntfs-3g,is ntfs-3g package installed?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="620"/>
        <source>there seem to be an open volume accociated with given address</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="621"/>
        <source>no file or device exist on given path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="622"/>
        <source>volume could not be opened with the presented key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="623"/>
        <source>insufficient privilege to mount the device with given options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="625"/>
        <source>only root user can perform this operation</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="626"/>
        <source>-O and -m options can not be used together</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="627"/>
        <source>could not create mount point, invalid path or path already taken</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="628"/>
        <source>shared mount point path aleady taken</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="629"/>
        <source>there seem to be an opened mapper associated with the device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="630"/>
        <source>could not get a passphrase from the module</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="631"/>
        <source>could not get passphrase in silent mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="632"/>
        <source>insufficient memory to hold passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="633"/>
        <source>one or more required argument(s) for this operation is missing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="634"/>
        <source>invalid path to key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="635"/>
        <source>could not get enought memory to hold the key file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="636"/>
        <source>insufficient privilege to open key file for reading</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="637"/>
        <source>could not get a passphrase through a local socket</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="638"/>
        <source>failed to mount a filesystem:invalid/unsupported mount option or unsupported file system encountered</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="639"/>
        <source>could not create a lock on /etc/mtab</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="640"/>
        <source>a non supported device encountered,device is missing or permission denied
Possible reasons for getting the error are:
1.Device path is invalid.
2.The device has LVM or MDRAID signature</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/password_dialog.cpp" line="642"/>
        <source>unrecognized ERROR with status number %1 encountered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>password_dialog</name>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="14"/>
        <source>lxqt wallet service</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="42"/>
        <source>The application &apos;%1&apos; has requested to open the wallet &apos;%2&apos;.Enter the password below for this wallet to unlock it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="74"/>
        <source>wallet &apos;%1&apos; does not exist,do you want to create it?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="90"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Wallet &lt;span style=&quot; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt; does not exist, do you want to create it?&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="107"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;An application &lt;span style=&quot; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt; has requested to open a wallet&lt;span style=&quot; font-weight:600;&quot;&gt; &apos;%2&apos;&lt;/span&gt;. Enter the password below for this wallet to unlock it.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="125"/>
        <source>Unlock</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="141"/>
        <source>Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="154"/>
        <source>ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/lxqt_wallet/frontend/password_dialog.ui" line="167"/>
        <source>Ok</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>walletconfig</name>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="17"/>
        <source>manage volumes in a wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="32"/>
        <source>&amp;add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="51"/>
        <source>&amp;delete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="64"/>
        <source>do&amp;ne</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="111"/>
        <source>volume id</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="116"/>
        <source>comment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.ui" line="121"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.cpp" line="73"/>
        <source>warning</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.cpp" line="73"/>
        <source>are you sure you want to delete a volume with an id of &quot;%1&quot;?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfig.cpp" line="135"/>
        <location filename="../../zuluCrypt-gui/walletconfig.cpp" line="169"/>
        <source>&lt;redacted&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>walletconfiginput</name>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="14"/>
        <source>add an entry to wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="26"/>
        <source>add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="39"/>
        <source>cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="64"/>
        <source>volume id</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="83"/>
        <source>comment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="102"/>
        <source>key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.ui" line="121"/>
        <source>repeat key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.cpp" line="78"/>
        <location filename="../../zuluCrypt-gui/walletconfiginput.cpp" line="84"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.cpp" line="78"/>
        <source>atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.cpp" line="84"/>
        <source>passphrases do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/walletconfiginput.cpp" line="104"/>
        <source>select a luks volume</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>zuluCrypt</name>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="20"/>
        <source>zuluCrypt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="80"/>
        <source>Encrypted volume path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="85"/>
        <source>Encrypted volume mount point path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="90"/>
        <source>Type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="106"/>
        <source>&amp;open</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="114"/>
        <source>&amp;create</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="122"/>
        <source>&amp;help</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="131"/>
        <source>&amp;luks</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="141"/>
        <source>o&amp;ptions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="155"/>
        <source>&amp;favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="160"/>
        <source>&amp;zC</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="185"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="204"/>
        <source>encrypted container in a file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="188"/>
        <source>Ctrl+Z</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="193"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="218"/>
        <source>encrypted container in a partition</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="196"/>
        <source>Ctrl+X</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="207"/>
        <source>Ctrl+A</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="221"/>
        <source>Ctrl+S</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="229"/>
        <source>about</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="232"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="386"/>
        <source>Ctrl+R</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="237"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="762"/>
        <source>add key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="240"/>
        <source>Ctrl+U</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="245"/>
        <source>delete key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="248"/>
        <source>Ctrl+W</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="253"/>
        <source>crypto info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="256"/>
        <source>Ctrl+E</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="261"/>
        <source>keyfile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="264"/>
        <source>Ctrl+D</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="269"/>
        <source>tray icon</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="272"/>
        <source>Ctrl+K</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="277"/>
        <source>select font</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="280"/>
        <source>Ctrl+L</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="285"/>
        <source>favorite volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="290"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="343"/>
        <source>manage favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="295"/>
        <source>select random number generator</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="298"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="370"/>
        <source>Ctrl+P</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="303"/>
        <source>close application</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="306"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="410"/>
        <source>Ctrl+C</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="311"/>
        <source>minimize</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="314"/>
        <source>Ctrl+T</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="319"/>
        <source>minimize to tray</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="322"/>
        <source>Ctrl+Y</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="327"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="259"/>
        <source>quit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="330"/>
        <source>Ctrl+Q</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="335"/>
        <source>close all opened volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="338"/>
        <source>Ctrl+G</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="346"/>
        <source>Ctrl+F</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="351"/>
        <source>erase data in a device</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="354"/>
        <source>Ctrl+N</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="359"/>
        <source>backup header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="362"/>
        <source>Ctrl+B</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="367"/>
        <source>restore header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="375"/>
        <source>permission</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="378"/>
        <source>Ctrl+I</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="383"/>
        <source>encrypt a file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="391"/>
        <source>decrypt a file</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="394"/>
        <source>Ctrl+H</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="399"/>
        <source>luks header backup</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="402"/>
        <source>Ctrl+J</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="407"/>
        <source>manage system volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="415"/>
        <source>manage volumes in internal wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="423"/>
        <source>configure wallets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="431"/>
        <source>manage non system volumes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="439"/>
        <source>manage volumes in kde wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="442"/>
        <source>Shift+V</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="447"/>
        <source>manage volumes in gnome wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="450"/>
        <source>Alt+V</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="455"/>
        <source>change internal wallet password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="458"/>
        <source>Ctrl+O</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="418"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="426"/>
        <source>Ctrl+V</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.ui" line="434"/>
        <source>Ctrl+M</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="535"/>
        <source>info</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="535"/>
        <source>resetting font size to %1 because larger font sizes do not fit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="613"/>
        <source>about zuluCrypt</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="618"/>
        <source>
All luks based encrypted volumes have what is called a &quot;luks header&quot;.

A luks header is responsible for storing information necessary to open a luks based volume and any damage to it will makes it impossible to open the volume causing permanent loss of encrypted data.

The damage to the header is usually caused by accidental formatting of the device or use of some buggy partitioning tools or wrongly reassembled logical volumes.

Having a backup of the luks header is strongly advised because it is the only way the encrypted data will be accessible again after the header is restored if the header on the volume get corrupted.

For more information, please read the FAQ at: http://code.google.com/p/cryptsetup/wiki/FrequentlyAskedQuestions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="628"/>
        <source>important information on luks header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="672"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="845"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="846"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="847"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="848"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="849"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="850"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="851"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="852"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="853"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="854"/>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="855"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="672"/>
        <source>volume is not open or was opened by a different user</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="674"/>
        <source>volume properties</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="700"/>
        <source>list is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="725"/>
        <source>warning</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="725"/>
        <source>could not open mount point because &quot;%1&quot; tool does not appear to be working correctly</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="748"/>
        <source>close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="752"/>
        <source>open folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="756"/>
        <source>properties</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="763"/>
        <source>remove key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="765"/>
        <source>backup luks header</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="785"/>
        <source>add to favorite</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="803"/>
        <source>cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="845"/>
        <source>close failed, volume is not open or was opened by a different user</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="846"/>
        <source>close failed, one or more files in the volume are in use.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="847"/>
        <source>close failed, volume does not have an entry in /etc/mtab</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="848"/>
        <source>close failed, could not get a lock on /etc/mtab~</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="849"/>
        <source>close failed, volume is unmounted but could not close mapper,advice to close it manually</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="850"/>
        <source>close failed, could not resolve full path of device
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="851"/>
        <source>close failed, shared mount point appear to be busy
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="852"/>
        <source>close failed, shared mount point appear to belong to a different user
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="853"/>
        <source>close failed, shared mount point appear to be in an ambiguous state,advice to unmount manually</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="854"/>
        <source>close failed, could not find any partition with the presented UUID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/zulucrypt.cpp" line="855"/>
        <source>unrecognized error with status number %1 encountered</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
