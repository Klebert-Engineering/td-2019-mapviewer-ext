<plugin
    name="mapviewer.extension.td2019"
    iid="org.nds-association.mapviewer.extension.td2019"
    version="0.1.0"
    compatVersion="0.1.0"
    disabledByDefault="false"
>

  <vendor>Navigation Data Standard e.V.</vendor>
  <copyright>Copyright (c) 2018, Navigation Data Standard e.V.</copyright>
  <license> </license>
  <category>Mapviewer</category>

  <description>
      Contains MapViewer extension to mark the most recently selected position
  </description>

  <url>http://www.nds-association.org/</url>

  <dependencyList>
      <dependency name="mapviewer.backend" version="0.2.1"/>
      <!-- Dummy dependency to make sure that the loading order is maintained, because loading order is visual order !-->
      <dependency name="mapviewer.ext.lanes" version="0.1.0"/>
  </dependencyList>

</plugin>
