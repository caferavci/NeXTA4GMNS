---
title: 'The Power of Visualization: Using NeXTA4GMNS for Trajectory Display'
tags:
  - Python
  - Transportation
  - Visualization
  - Trajectories
  - GMNS
  - Traffic Simulation
authors:
  - name: Cafer Avci
    orcid: 0000-0000-0000-0000
    affiliation: 1
    corresponding: true
  - name: Yajun Liu
    orcid: 0000-0000-0000-0000
    affiliation: 2
  - name: Xuesong (Simon) Zhou
    orcid: 0000-0000-0000-0000
    affiliation: 2
affiliations:
  - name: School of Civil and Environmental Engineering, Cornell University, Ithaca, NY, 14853, USA
    index: 1
  - name: School of Sustainable Engineering and the Built Environment, Arizona State University, Tempe, AZ, 85281, USA
    index: 2
date: 1 May 2025
bibliography: paper.bib
---

# Summary

Contemporary transportation modeling increasingly relies on large and heterogeneous data sources, including multi-resolution network topologies, time-varying traffic states, microscopic vehicle trajectories, and large-scale simulation outputs. Making sense of these data requires visualization tools that are both accurate and usable by researchers and practitioners.

NeXTA4GMNS (NeXTA) is an open-source platform for transportation network and trajectory visualization built around the General Modeling Network Specification (GMNS) standard [@GMNS2020]. NeXTA provides editable, multi-resolution node-link representations of transportation networks and supports analysis of link performance metrics such as travel time, traffic volume, and speed across time horizons. Beyond link-level metrics, NeXTA supports visualization and exploration of vehicle trajectories, tensor-based data structures for spatiotemporal processing, and shortest path computations. Unlike general-purpose GIS software, NeXTA is designed for scientific transportation modeling workflows, including trajectory analysis, OD trip and path exploration, and integration with simulation and assignment frameworks such as FHWA’s AMS and dynamic traffic assignment (DTA) models [@yelchuru2017analysis].

# Statement of Need

Transportation systems have become increasingly complex, and traditional data collection and analysis approaches (manual counts, loop detectors, and periodic surveys) often lack the spatial and temporal resolution required to understand modern travel dynamics and to support modeling, validation, and policy analysis [@Papageorgiou1991]. In parallel, the GMNS community has emphasized the importance of standardized network representations and the need for visualization platforms capable of working at the scale and complexity of real-world datasets [@GMNS2020]. Tools that integrate network topology, traffic dynamics, and simulation outputs are especially important when analysts must iterate quickly between calibration, validation, and scenario testing.

NeXTA was developed to address these needs as a GMNS-compliant, open-source visualization platform for transportation network analysis and trajectory interpretation. Rather than focusing only on map rendering, NeXTA supports multi-resolution node-link representations and incorporates tensor-based data structures to enable analysis of spatiotemporal traffic patterns grounded in traffic flow theory [@Lighthill1955; @Richards1956].

A common alternative for visualization and editing is general-purpose GIS software such as QGIS. However, NeXTA’s transportation-specific focus provides built-in support for trajectory workflows and simulation outputs that typically require additional plugins and preprocessing in GIS-first toolchains.

| Feature | NeXTA | QGIS |
|---|---|---|
| Network Representation | GMNS-compliant, node-link model | GIS shapefiles and layers |
| Trajectory Analysis | Built-in support for microscopic trajectory data (e.g., NGSIM) | Requires external plugins and preprocessing |
| Traffic Simulation Integration | Native support for simulation outputs (e.g., DTALite [@Zhou2012DTALite], SUMO with post-processing [@Behrisch2011SUMO]) | Limited support; not transportation-specific |
| Network Editing | Editable, hierarchical, and multi-resolution network views | Basic vector layer editing, not traffic-centric |
| Tensor-Based Modeling | Supports tensor structures for trajectory processing | Not available |
| Usability for Transportation Practitioners | Tailored for planners, researchers, and educators in traffic modeling | General-purpose GIS users |

NeXTA is particularly useful for exploring traffic dynamics, examining microscopic trajectories, and modifying network structures for simulation inputs or scenario development. High-resolution datasets such as Next Generation Simulation (NGSIM) [@NGSIM2007] provide detailed trajectories that are difficult to interpret without specialized software. NeXTA enables interactive exploration of such datasets, supporting research and practice in transportation modeling and simulation.

# Software Description

## Core Features and Capabilities

**Multi-resolution network visualization and editing**

NeXTA supports transitions between levels of detail, from link-level inspection for microsimulation to corridor and zonal views for planning. Users can edit and analyze networks at the link level, aggregate toward path-level analysis for route choice studies, and move toward zonal-level summaries within the same interface.

**Tensor-based representation of trajectories**

NeXTA represents trajectories as multi-dimensional tensors rather than as simple sequences of points. This representation enables compact storage, efficient manipulation, and analytical operations that support spatiotemporal slicing, filtering, and aggregation.

**Cross-resolution and multi-view trajectory visualization**

NeXTA supports simultaneous visualization of individual trajectories and aggregate flow patterns across the network, helping users relate microscopic driving behavior to macroscopic phenomena that are central to traffic flow theory [@Gazis2002; @Treiber2013].

**Integration with simulation frameworks**

NeXTA integrates with traffic simulation and assignment tools to streamline visualization of outputs and iterative workflows. This aligns with FHWA guidance emphasizing integration among analysis, modeling, and simulation tools [@nevers2013effective]. This integration is useful for evaluating the impacts of control policies and infrastructure modifications in network management contexts [@Papageorgiou2003].

## Multi-dimensional Matrix Representations

NeXTA models agent movements using a trajectory tensor $X(a)$ capturing key attributes for each vehicle or traveler, including:

- unique identity and classification
- origin-destination (OD) information
- route and path specification
- departure and arrival timing
- vehicle and behavior parameters

To support robust optimization and calibration, NeXTA maintains a baseline prior estimate $X_h(a)$ that can provide regularization when data are noisy or incomplete. In addition, a measurement tensor $Y(i, j, t, m)$ tracks link-level states, including flow, density, speed, and other performance metrics. Mapping between individual-level trajectories and aggregate network conditions follows established ideas from traffic assignment theory [@Wardrop1952; @Beckmann1956].

## Computational Graph for Traffic Modeling

NeXTA structures key OD-to-link relationships using computational graphs, enabling forward and backward propagation for optimization-based calibration of travel times and flows. This layered structure is illustrated in \autoref{fig:CompGraph}.

![Layered computational graph representing OD-to-link relationships through path-based assignments and travel time transformations.\label{fig:CompGraph}](MathematicalModel.png)

In Layer 1, OD flows $F_{OD}$ are converted into path flows $f_P$ via an OD-to-Path assignment matrix $B$:
$f_P = B \cdot F_{OD}$.
This represents route choice behavior and connects to discrete choice concepts [@McFadden1974; @Ben-Akiva1985].

In Layer 2, path flows are aggregated into link flows $f_L$ using a Path-to-Link incidence matrix $A$:
$f_L = A \cdot f_P$.

In Layer 3, link travel times $T_L$ are aggregated into path travel times $T_P$:
$T_P = A^\top \cdot T_L$.

In Layer 4, path travel times are mapped back to OD-level travel times:
$T_{OD} = B \cdot T_P$.

This computational organization supports gradient-based workflows and creates a direct mapping between modeling elements and the visualization features available in NeXTA, as summarized below.

| Modeling Element | NeXTA Visualization Feature |
|---|---|
| OD Flows ($F_{OD}$) | Interactive OD pair filtering and summary statistics |
| Path Flows ($f_P$) | Route and path analysis view |
| Link Flows ($f_L$) | Link performance view (flow, speed, density) |
| Link Travel Times ($T_L$) | Dynamic heatmaps and link-level metrics dashboard |
| Path Travel Times ($T_P$) | Aggregated trajectory visualizations with time profiles |
| OD Travel Times ($T_{OD}$) | OD-level average travel time matrices |
| Tensor Representation ($X(a)$) | Multi-dimensional filtering and agent tracking |

# Practical Applications and Examples

## OD-Based Trajectory Analysis

NeXTA provides OD pair and path-based trajectory filtering to explore travel behavior across spatial and temporal dimensions (Figure \autoref{fig:ODFiltering}). Users can filter trajectories by OD zones, departure time windows, and vehicle classes to examine specific demand slices such as freight movements or peak-period congestion.

These filtering workflows align with NeXTA’s layered analysis structure. OD filtering corresponds to Layer 1, where flows are interpreted as path choices. Users can then examine Layer 2 effects by relating filtered trajectories to link-level congestion and performance metrics. Layers 3 and 4 support analysis of how link-level conditions accumulate along paths and aggregate into OD-level summaries.

![OD Pair and Path-based vehicle trajectory filtering in NeXTA, showing the interface for analyzing travel patterns across different origin-destination pairs.\label{fig:ODFiltering}](NeXTAmultiLayer.jpg)

NeXTA’s route-based visualization supports practices described in FHWA’s *Trajectory Processor User’s Guide* [@FHWA2011] while adding a GMNS-compliant environment that connects OD filtering with path analysis and link performance metrics.

## High-Resolution Empirical Data Analysis

NeXTA supports detailed exploration of high-resolution empirical trajectory datasets. Figure \autoref{fig:NGSIMNeXTA} illustrates an example using the NGSIM I-101 dataset, with lane, speed, and spacing filters applied to highlight vehicle behavior patterns.

![2D NGSIM 101 Trajectory Visualization of Lane 2 with distance and speed selection filters applied to highlight specific vehicle behavior patterns.\label{fig:NGSIMNeXTA}](NeXTaNGSIM.jpg)

Trajectory-level visualization supports nuanced validation and calibration of microsimulation and behavioral models, complementing aggregate statistics that can obscure important dynamics. NeXTA’s support for empirical data also enables downstream applications such as traffic prediction modeling [@KIM2020102786], calibration of connected and automated vehicle models [@Shladover2012], and evaluation of traffic flow theories under observed conditions.

## Demonstration and Learning Resources

NeXTA includes demonstration and learning materials designed for researchers, practitioners, and educators. A walkthrough video is available on YouTube: \url{https://www.youtube.com/watch?v=example_NeXTA_demo}. The video demonstrates network editing, OD filtering, trajectory visualization, and tensor-based exploration features.

# Impact and Future Directions

NeXTA aims to make sophisticated transportation analysis accessible by combining rigorous modeling foundations with interactive visualization. By connecting modeling elements (OD, path, and link layers) to visualization views, NeXTA supports calibration, validation, and communication of results to technical and non-technical audiences.

Future directions include deeper integration of machine learning with traffic flow theory for improved modeling [@Zhang2011], leveraging connected vehicle data for real-time analysis and control [@Talebpour2016], and extending support for multimodal systems and mode interactions [@Cats2017]. NeXTA’s extensible architecture is intended to accommodate these needs while maintaining its focus on GMNS compliance and trajectory-centric analysis.

# Acknowledgements

The authors thank the anonymous reviewers for their thoughtful feedback, which significantly improved this paper. The authors also acknowledge the GMNS community for ongoing support and input throughout NeXTA’s development. The authors additionally thank the Federal Highway Administration for making the NGSIM dataset publicly available, enabling validation and demonstration workflows used in this work.
