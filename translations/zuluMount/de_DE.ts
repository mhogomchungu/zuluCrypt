<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.0" language="de_DE" sourcelanguage="en_US">
<context>
    <name>DialogMsg</name>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="17"/>
        <source>Dialog</source>
        <translation>Dialog</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="29"/>
        <source>&amp;ok</source>
        <translation>&amp;Ok</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="42"/>
        <source>&amp;yes</source>
        <translation>&amp;Ja</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="55"/>
        <source>&amp;no</source>
        <translation>&amp;Nein</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="68"/>
        <source>text</source>
        <translation>Text</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="90"/>
        <source>type</source>
        <translation>Typ</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="109"/>
        <source>cipher</source>
        <translation>Algorithmus</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="125"/>
        <source>key size</source>
        <translation>Schlüsselgröße</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="141"/>
        <source>device</source>
        <translation>Gerät</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="160"/>
        <source>loop</source>
        <translation>Loop</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="179"/>
        <source>offset</source>
        <translation>Versatz</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="195"/>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="243"/>
        <source>size</source>
        <translation>Größe</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="211"/>
        <source>mode</source>
        <translation>Modus</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="227"/>
        <source>fs</source>
        <translation>FS</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="259"/>
        <source>used</source>
        <translation>Benutzt</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="275"/>
        <source>unused</source>
        <translation>Ungenutzt</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="291"/>
        <source>used %</source>
        <translation>Benutzt %</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.ui" line="307"/>
        <source>active slots</source>
        <translation>Aktive Slots</translation>
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
        <translation>Standardmäßig werden Laufwerke mit der &quot;noexec&quot; Option eingehangen, wodurch es nicht möglich ist Programme 
oder Skripte von diesen auszuführen. Wenn Sie dies vermeiden wollen, erstellen Sie eine Gruppe &quot;zulumount-exec&quot; and fügen Sie sich zu dieser Gruppe hinzu, dadurch werden Laufwerke mit der &quot;exec&quot;-Option eingehangen, was dazu führt, dass Programme und Skripte ausgeführt werden können.

Wenn Sie es bevorzugen, dass ein Gerät nicht als Systemgerät erkannt wird, starten Sie das Programm

als root und gehen dann im Menü auf &quot;Optionen -&gt; Nicht-Systemlaufwerke verwalten&quot;.

Fügen Sie das Gerät zur Liste hinzu und es wird fortan nicht mehr als &quot;Systemgerät&quot; behandelt.

Alternativ können Sie sich auch zur Gruppe &quot;zulucrypt&quot; hinzufügen, wodurch alle Beschränkungen aufgehoben werden.</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="99"/>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="115"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="107"/>
        <source>insufficient privilege to access a system device,
only root user or members of group zulucrypt can do that</source>
        <translation>Sie haben unzureichende Rechte um auf ein Systemlaufwerk zuzugreifen,
nur der Root Benutzer und Mitglieder der Gruppe zulucrypt dürfen dies machen.</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="109"/>
        <source>insufficient privilege to access a system device in read/write mode,
only root user or members of group zulucrypt-write can do that</source>
        <translation>Sie haben unzureichende Rechte um auf ein Systemlaufwerk im Lese-/Schreibmodus zuzugreifen,
nur der Root Benutzer und Mitglieder der Gruppe zulucrypt dürfen dies machen.</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="112"/>
        <source>you do not seem to have proper permissions to access the encrypted file in %1 mode,check file permissions and try again</source>
        <translation>Sie scheinen nicht genügend Rechte für den Zugriff auf die verschlüsselte Datei im %1 zu haben. Überprüfen Sie die Dateirechte und probieren Sie es erneut.</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/dialogmsg.cpp" line="272"/>
        <source>do not show this dialog again</source>
        <translation>Diesen Dialog nicht erneut zeigen.</translation>
    </message>
</context>
<context>
    <name>MainWindow</name>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="17"/>
        <source>zuluMount</source>
        <translation>zuluMount</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="55"/>
        <source>volume path</source>
        <translation>Laufwerkspfad</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="63"/>
        <source>mount point path</source>
        <translation>Einhängepunkt</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="71"/>
        <source>file system</source>
        <translation>Dateisystem</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="79"/>
        <source>label</source>
        <translation>Beschriftung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="87"/>
        <source>size</source>
        <translation>Größe</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="95"/>
        <source>%used</source>
        <translation>% benutzt</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="133"/>
        <source>&amp;mount file</source>
        <translation>&amp;Datei einhängen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="152"/>
        <source>&amp;refresh</source>
        <translation>&amp;Aktualisieren</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.ui" line="165"/>
        <source>&amp;close</source>
        <translation>&amp;Schließen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="111"/>
        <source>automount volumes</source>
        <translation>Laufwerke autmatisch einhängen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="121"/>
        <source>auto open mount point</source>
        <translation>Einhängepunkt automatisch öffnen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="126"/>
        <source>quit</source>
        <translation>Beenden</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="320"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="327"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="334"/>
        <source>LABEL=&quot;%1&quot;</source>
        <translation>Beschriftung=&quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="336"/>
        <source>LABEL=&quot;%1&quot;
%2</source>
        <translation>Beschriftung=&quot;%1&quot;
%2</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="397"/>
        <source>mount</source>
        <translation>Einhängen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="402"/>
        <source>unmount</source>
        <translation>Aushängen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="404"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="420"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="425"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="428"/>
        <source>properties</source>
        <translation>Eigenschaften</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="408"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="422"/>
        <source>open folder</source>
        <translation>Verzeichnis öffnen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="411"/>
        <source>open private folder</source>
        <translation>Privates Verzeichnis öffnen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="413"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="430"/>
        <source>open shared folder</source>
        <translation>Gemeinsames Verzeichnis öffnen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="437"/>
        <source>close menu</source>
        <translation>Menü schließen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="474"/>
        <source>warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="470"/>
        <source>could not open mount point because &quot;%1&quot; tool does not appear to be working correctly</source>
        <translation>Der Einhängepunkt kann nicht geöffnet werden, weil das &quot;%1&quot; Werkzeug nicht richtig zu funktionieren scheint.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="515"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="522"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="659"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="808"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="897"/>
        <source>ERROR</source>
        <translation>FEHLER</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="516"/>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="523"/>
        <source>could not get volume properties.
volume is not open or was opened by a different user</source>
        <translation>Die Laufwerkseigenschaften konnten nicht gelesen werden.
Das Laufwerk ist nicht geöffnet oder es wurde von einem anderen Benutzer geöffnet.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="520"/>
        <source>volume properties</source>
        <translation>Laufwerkseigenschaften</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="660"/>
        <source>permission to access the volume was denied
or
the volume is not supported
(LVM/MDRAID signatures found)</source>
        <translation>Sie haben keine Berechtigung zum Zugriff auf das Laufwerk,
oder das Laufwerk wird nicht unterstützt
(LVM/MDRAID Signatur gefunden).</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="669"/>
        <source>select an image file to mount</source>
        <translation>Wählen Sie eine Abbild-Datei zum Einhängen.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mainwindow.cpp" line="809"/>
        <source>reading partition properties took longer than expected and operation was terminated,click refresh to try again</source>
        <translation>Das Lesen der Partitionseigenschaften dauert länger als erwartet, deshalb wurde die Operation abgebrochen.
Klicken Sie Aktualisieren und versuchen Sie es erneut.</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="246"/>
        <source>setting this option will cause the volume to open in read only mode</source>
        <translation>Durch das Aktivieren dieser Option wird das Laufwerk nur im Lesemodus geöffnet.</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="259"/>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="270"/>
        <source>info</source>
        <translation>Information</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="76"/>
        <source>
options:
	-d   path to where a volume to be auto unlocked/mounted is located
	-m   tool to use to open a default file manager(default tool is xdg-open)
</source>
        <translation>Optionen:
	-d   Pfad zu einem Laufwerk, dass automatisch entsperrt/eingehängt werden soll
	-m   Werkzeug zum öffnen des Standard-Dateimanagers (Das Standardprogramm ist xdg-open)</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="82"/>
        <source>	-e   start the application without showing the GUI
</source>
        <translation>	-e   Startet das Programm ohne die grafische Oberfläche anzuzeigen</translation>
    </message>
    <message>
        <location filename="../../zuluCrypt-gui/utility.cpp" line="93"/>
        <source>if the option is checked,a primary private mount point will be created in &quot;/run/media/private/$USER/&quot;
and a secondary publicly accessible &quot;mirror&quot; mount point will be created in &quot;/run/media/public/&quot;</source>
        <translation>Wenn diese Option gewählt ist, wird ein privater Einhängepunkt unter /run/media/private/$USER/&quot; erstellt
und ein zweiter öffentlich zugänglicher &quot;Spiegel&quot;-Einhängepunkt unter /run/media/public/&quot;</translation>
    </message>
</context>
<context>
    <name>keyDialog</name>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="14"/>
        <source>unlock and mount a luks volume</source>
        <translation>Ein Luks-Laufwerk entsperren und einhängen.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="26"/>
        <source>&amp;open</source>
        <translation>&amp;Öffnen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="42"/>
        <source>&amp;cancel</source>
        <translation>&amp;Abbrechen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="55"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="444"/>
        <source>key</source>
        <translation>Schlüssel</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="86"/>
        <source>&amp;key</source>
        <translation>&amp;Schlüssel</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="99"/>
        <source>key &amp;from a key file</source>
        <translation>Schlüssel aus einer Datei</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="112"/>
        <source>&amp;plugin</source>
        <translation>&amp;Erweiterung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="149"/>
        <source>mount name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="204"/>
        <source>mount in &amp;read only mode</source>
        <translation>Nur Lesemodus</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.ui" line="217"/>
        <source>&amp;share mount point</source>
        <translation>Gemeinsamer Einhängepunkt</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="60"/>
        <source>unlock and mount a luks volume in &quot;%1&quot;</source>
        <translation>Ein Luks-Laufwerk entsperren und in &quot;%1&quot; einhängen.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="62"/>
        <source>unlock and mount an encrypted volume in &quot;%1&quot;</source>
        <translation>Ein verschlüsseltes Laufwerk entsperren und in &quot;%1&quot; einhängen.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="115"/>
        <source>plugin path</source>
        <translation>Pfad zur Erweiterung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="117"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="432"/>
        <source>plugin name</source>
        <translation>Name der Erweiterung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="124"/>
        <source>select a folder to create a mount point in</source>
        <translation>Wählen Sie ein Verzeichnis zum Erstellen eines Einhängepunktes.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="171"/>
        <source>select a file to be used as a keyfile</source>
        <translation>Wählen Sie eine Datei, die als Schlüsseldatei dienen soll.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="228"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="237"/>
        <source>cancel</source>
        <translation>Abbrechen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="253"/>
        <source>warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="253"/>
        <source>could not open mount point because &quot;%1&quot; tool does not appear to be working correctly</source>
        <translation>Der Einhängepunkt kann nicht geöffnet werden, weil das &quot;%1&quot; Werkzeug nicht richtig zu funktionieren scheint.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="279"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="286"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="298"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="366"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="371"/>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="380"/>
        <source>ERROR</source>
        <translation>FEHLER</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="279"/>
        <source>An error has occured and the volume could not be opened</source>
        <translation>Ein Fehler ist aufgetreten und das Laufwerk konnte nicht geöffnet werden.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="298"/>
        <source>the volume does not appear to have an entry in the wallet</source>
        <translation>Das Laufwerk scheint keinen Eintrag in der Brieftasche zu haben.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="366"/>
        <source>plug in name field is empty</source>
        <translation>Das Feld mit dem Namen der Erweiterung ist leer.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="371"/>
        <source>keyfile field is empty</source>
        <translation>Das Feld mit der Schlüsseldatei ist leer.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="380"/>
        <source>&quot;/&quot; character is not allowed in the mount name field</source>
        <translation>Das Zeichen &quot;/&quot; ist im Namensfeld nicht erlaubt.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/keydialog.cpp" line="457"/>
        <source>keyfile path</source>
        <translation>Pfad zur Schlüsseldatei</translation>
    </message>
</context>
<context>
    <name>mountPartition</name>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="14"/>
        <source>select mount point path</source>
        <translation>Einhängepunkt wählen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="26"/>
        <source>mount name</source>
        <translation>Name</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="45"/>
        <source>&amp;mount</source>
        <translation>&amp;Einhängen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="77"/>
        <source>use la&amp;bel</source>
        <translation>Beschriftung benutzen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="100"/>
        <source>&amp;cancel</source>
        <translation>&amp;Abbrechen</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="116"/>
        <source>mount &amp;read only</source>
        <translation>Nur Lesemodus</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.ui" line="129"/>
        <source>&amp;share mount point</source>
        <translation>Geteilter Einhängepunkt</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.cpp" line="125"/>
        <location filename="../../zuluMount-gui/mountpartition.cpp" line="215"/>
        <source>ERROR</source>
        <translation>FEHLER</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.cpp" line="125"/>
        <source>&quot;/&quot; character is not allowed in the mount name field</source>
        <translation>Das Zeichen &quot;/&quot; ist im Namensfeld nicht erlaubt.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.cpp" line="152"/>
        <source>Select Path to mount point folder</source>
        <translation>Pfad zum Einhängepunkt wählen.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.cpp" line="205"/>
        <source>warning</source>
        <translation>Warnung</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/mountpartition.cpp" line="205"/>
        <source>could not open mount point because &quot;%1&quot; tool does not appear to be working correctly</source>
        <translation>Der Einhängepunkt kann nicht geöffnet werden, weil das &quot;%1&quot; Werkzeug nicht richtig zu funktionieren scheint.</translation>
    </message>
</context>
<context>
    <name>oneinstance</name>
    <message>
        <location filename="../../zuluMount-gui/oneinstance.cpp" line="43"/>
        <source>There seem to be another instance running,exiting this one</source>
        <translation>Es scheint eine weiter Instanz ausgeführt zu werden. Diese wird nun beendet.</translation>
    </message>
    <message>
        <location filename="../../zuluMount-gui/oneinstance.cpp" line="45"/>
        <source>Previous instance seem to have crashes,trying to clean up before starting</source>
        <translation>Die letzte Instanz scheint durch eine Crash beendet wurden zu sein, es wird versucht vor dem Start aufzuräumen.</translation>
    </message>
</context>
</TS>
