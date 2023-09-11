---
title: 'The Power of Visualization:Using NeXTa4GMNS for Trajectory Display'
tags:
  - Transportation Network
  - Trajectory
  - General Modeling Network Specification
  - Visualization
authors:
  - name: Cafer Avci
    equal-contrib: true
    affiliation: 1
  - name: Xuesong (Simon) Zhou
    equal-contrib: true
    affiliation: 2
affiliations:
 - name: School of Civil and Environmental Engineering, Cornell University, Ithaca, NY, 14853, USA
   index: 1
 - name: School of Sustainable Engineering and the Built Environment, Arizona State University, Tempe, AZ, 85281, USA
   index: 2
date: 01 October 2023
bibliography: paper.bib

---

# Summary

# Statement of need


In the field of transportation analysis and modeling, the essential integration of tools and technologies has become critical for obtaining comprehensive insights into intricate urban mobility systems. Traffic flow studies, one of the core aspects of this complexity, examine the interactions between diverse traffic elements and infrastructure to elucidate how individual participants shape traffic patterns, relying extensively on precise real-world data for empirical research.

Researchers have been working to improve traffic flow measurements since the 1920s, initially relying on manual methods like stopwatches and making assumptions that may not align with modern congested traffic scenarios due to limitations in data collection technology. While new methods like radar-based devices and loop detectors have limitations in capturing comprehensive traffic flow details, recent technologies such as onboard radar detectors, cameras, and laser sensors offer more accurate data but are still limited in scope and monitored vehicles. Advances in traffic monitoring now involve GPS-equipped probe vehicles that upload real-time data about their position, speed, and direction via wireless communication, but they too have limitations in capturing complete traffic flow dynamics. This evolution in traffic measurement methods has transitioned from high-cost aerial photography to the use of drones for trajectory data collection, and the deployment of roadside video cameras and video-based traffic flow monitoring systems has provided valuable high-accuracy data for traffic flow studies, with the the next generation simulation (NGSIM) dataset being a widely utilized resource in this field.





In general, the software suite of NeXTA4GMNS aims to:

(1) Provide an open-source code base to enable transportation researchers and software developers to expand its range of capabilities to various traffic management application.

(2) Present results to other users by visualizing time-varying traffic flow dynamics and traveler route choice behavior in an integrated environment.

(3) Provide a free, educational tool for students to understand the complex decision-making process in transportation planning and optimization processes.

(4) By managing GMNS data sets in both QGIS and NeXTA platforms, users can visualize the background GIS map for a GMNS network, in a broader spatial context, while NeXTA can provide time-dependent link performance visualization, path-level and agent-level analysis, and time-dependent agent trajectory visualization.



This paper presents a comprehensive guide to harnessing the combined power of GMNS (General Mobility Network Specification), AMS (Analysis, Modeling, and Simulation), QGIS (Quantum Geographic Information System), and NeXTA (Network Extraction from Textual Annotations) for advanced transportation analysis.


This paper serves as an invaluable resource for transportation professionals, urban planners, and researchers seeking a holistic understanding of GMNS-based transportation analysis, visualization, and modeling. Through a blend of theory and practical guidance, it empowers users to leverage GMNS, AMS, QGIS, and NeXTA to unlock the potential of rich transportation data and enhance decision-making in urban mobility systems.

This abstract discusses the integration of General Travel Network Format Specification (GMNS), Analysis Modeling Simulation (AMS), Quantum Geographic Information System (QGIS), and NeXTA in the field of transportation network analysis. GMNS, developed by the Zephyr Foundation, promotes flexibility and efficiency in supporting transportation innovations. AMS, endorsed by FHWA, plays a crucial role in addressing transportation system complexities and evaluating solutions driven by emerging technologies. QGIS is a free and open-source geospatial tool that empowers transportation professionals for spatial data analysis. NeXTA aims to provide an open-source code base for transportation researchers and software developers, enable result presentation through traffic dynamics visualization, serve as an educational resource for transportation planning, and manage GMNS datasets in both QGIS and NeXTA, offering spatial context and time-dependent analysis. This document explains the process of obtaining GMNS-compatible files (node.csv, link.csv, timing.csv, agent.csv, link_performance.csv) from OpenStreetMap for use in QGIS and NeXTA, enhancing transportation network analysis capabilities.




This document describes the process of obtaining [node.csv, link.csv, etc] GMNS-compatible files for use in QGIS from an OSM network and how to display GMNS file including node.csv, link.csv, timing.csv, agent.csv and link_performance.csv in NeXTA.




# Citations

Citations to entries in paper.bib should be in
[rMarkdown](http://rmarkdown.rstudio.com/authoring_bibliographies_and_citations.html)
format.

If you want to cite a software repository URL (e.g. something on GitHub without a preferred
citation) then you can do it with the example BibTeX entry below for @fidgit.

For a quick reference, the following citation commands can be used:
- `@author:2001`  ->  "Author et al. (2001)"
- `[@author:2001]` -> "(Author et al., 2001)"
- `[@author1:2001; @author2:2001]` -> "(Author1 et al., 2001; Author2 et al., 2002)"

# Figures

Figures can be included like this:
![Caption for example figure.\label{fig:example}](figure.png)
and referenced from text using \autoref{fig:example}.

Figure sizes can be customized by adding an optional second parameter:
![Caption for example figure.](figure.png){ width=20% }

# Acknowledgements

The authors would like to thank the anonymous reviewers for their helpful suggestions to improve this paper.

# References

