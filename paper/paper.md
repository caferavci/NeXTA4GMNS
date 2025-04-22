---
title: 'The Power of Visualization: Using NeXTa4GMNS for Trajectory Display'
tags:
  - Transportation Network
  - Trajectory
  - General Modeling Network Specification
  - Visualization
authors:
  - name: Cafer Avci
    equal-contrib: true
    affiliation: 1  
  - name: Yajun Liu
    equal-contrib: true
    affiliation: 2
  - name: Xuesong (Simon) Zhou
    equal-contrib: true
    affiliation: 2
affiliations:
 - name: School of Civil and Environmental Engineering, Cornell University, Ithaca, NY, 14853, USA
   index: 1
 - name: School of Sustainable Engineering and the Built Environment, Arizona State University, Tempe, AZ, 85281, USA
   index: 2
date: May 1, 2023
bibliography: nexta.bib

---

# Summary

The GMNS network community requires tools for network visualization, tra-
jectory visualization, and link performance assessment. Nexta, an open-source
tool, provides the basic node-link structure, allowing users to edit nodes and
links using the standard GMNS format. It also supports FHWA analysis and
AMS simulation modeling frameworks by enabling visualization of network com-
ponents, trajectories, and shortest paths. Nexta offers advantages over QGIS
by focusing on trajectory visualization and network debugging, allowing users
to operate and refine model networks for optimal performance.
Nexta further provides link performance and moving trajectory visualization,
helping users understand tensor concepts more easily. Since tensors are mul-
tidimensional arrays, visualizing these structures and their trajectories within
Nexta simplifies interpretation. With capabilities for OD trip, OD path, and
vehicle path visualization, users can classify each engine, destination, and OD
pair, observing trip lines and obtaining multi-view tensor visualizations. With-
out such tools, understanding tensors in this context is challenging.

# Statement of need

In the field of transportation analysis and modeling, the essential integration of tools and technologies has become critical for obtaining comprehensive insights into intricate urban mobility systems. Traffic flow studies, one of the core aspects of this complexity, examine the interactions between diverse traffic elements and infrastructure to elucidate how individual participants shape traffic patterns, relying extensively on precise real-world data for empirical research.

Since the 1920s, researchers have been striving to improve traffic flow measurements. Early methods were manual, often involving stopwatches and assumptions that no longer align with today’s congested traffic conditions due to limitations in data collection. Although more recent technologies such as radar-based devices and loop detectors have improved measurement capabilities, they still fall short in capturing the full dynamics of traffic flow. Innovations such as onboard radar detectors, cameras, and laser sensors offer higher accuracy but remain limited in scope and coverage.

The latest advances include the use of GPS-equipped probe vehicles, which transmit real-time data on position, speed, and direction via wireless communication. However, even these methods have limitations in representing comprehensive traffic behavior. The evolution of traffic measurement has moved from costly aerial photography to drones for trajectory data collection. Additionally, roadside video cameras and video-based traffic flow monitoring systems now offer high-accuracy data. The Next Generation Simulation (NGSIM) dataset, for instance, has become a widely used resource for studying microscopic traffic dynamics.

As traffic monitoring technologies have matured, there has been a growing need for software tools capable of organizing, visualizing, and analyzing this increasingly rich and complex data. This need is especially prominent within the GMNS (General Modeling Network Specification) community, which focuses on standardized representations of transportation networks. Addressing these requirements, **Nexta**, an open-source tool, provides a foundational node-link structure that allows users to edit networks using the GMNS standard. It supports FHWA analysis and AMS simulation modeling frameworks by enabling detailed visualization of network components, vehicle trajectories, and shortest paths.

Nexta, an open-source tool, provides a comprehensive node-link structure for editing networks using the GMNS standard, supporting FHWA analysis and AMS modeling frameworks. In comparison to QGIS, Nexta enhances trajectory visualization and model network debugging, offering improved link performance analysis for optimal network tuning. Key features of Nexta include:

- Multi-resolution network support
- Tensor-based representation of trajectories
- Cross-resolution and multi-view trajectory visualization capabilities
- Integration with scientific computing frameworks

Nexta further provides link performance and moving trajectory visualization, helping users understand tensor concepts more easily. Since tensors are multidimensional arrays, visualizing these structures and their trajectories within Nexta simplifies interpretation. With capabilities for OD trip, OD path, and vehicle path visualization, users can classify each engine, destination, and OD pair, observing trip lines and obtaining multi-view tensor visualizations. Without such tools, understanding tensors in this context is challenging.

While Nexta is widely used, a deep understanding of visualization infrastructure is required to fully leverage its scientific computing applications. For scientific computing and planning, Nexta allows users to edit, save network layers, and view multi-resolution networks, Markov chains, MISO, and various transit networks. Its capacity for network visualization, layer editing, and attribute saving makes it an invaluable scientific and educational tool.

Nexta also supports mathematical modeling of GPS data, including trajectory visualization and GPS point overlays, offering a focused view on transportation networks rather than solely on link mapping. Key features include agent trajectory visualization, route assignment by zone, selection percentages, and sub-area filtering. Proper organization and user guidance are essential to help others utilize Nexta effectively.

For planning and educational purposes, Nexta should be a standard tool in university transportation planning classes. It allows students and professionals to interact with a network beyond the surface level seen in Google Maps, providing an editable and operational view of networks that supports hands-on exploration and understanding.

## Trajectory Tensor Representation

The trajectory tensor $X(a)$ represents each agent’s movements across the network, capturing essential information such as origin-destination (OD) pairs, routes, trip details, and travel times. Each agent trajectory tensor includes:

- Agent identity
- Origin-Destination (OD) pair
- Route and trip specifics
- Departure and arrival times

## Measurement Tensor and Proportional Mapping

The measurement tensor $Y(i, j, t, m)$ provides real-time data on link states, including flows, densities, and speeds. This tensor integrates the following matrices for trajectory-to-measurement mapping:

- **Static Proportional Tensor Matrix (SPTM)**: Maps agent trajectories to specific routes or lanes.
- **Time-Dependent Link Proportional Tensor (TDLP)**: Adjusts trajectory mappings based on temporal changes in flow and density.

A prior estimate tensor $X_h(a)$ is used as a baseline for trajectory estimation.

## Mathematical Modeling and Optimization

We introduce mathematical models essential for Nexta’s framework.

### Flow Dynamics

Define:

- $Q(i, j, t)$: Flow on link $(i, j)$ at time $t$,
- $C(i, j)$: Capacity of link $(i, j)$,
- $VOC(i, j, t) = \frac{Q(i, j, t)}{C(i, j)}$:Volume-over-capacity ratio.



Traffic flow is limited by link capacity:

$$
Q(i, j, t) \leq C(i, j)
$$

### Trajectories

Trajectories are generated from transfer assignments using tools like DTALite and Path4GMNS. The trajectory data, captured from DLSim, includes loop detector data formulated as link performance metrics. These metrics allow for the verification of path connectivity and show the directionality of each link, enabling a structured approach to formalizing network modeling tools for scientific computing applications.

The travel time $T(i, j, t)$ depends on the VOC ratio:

$$
T(i, j, t) = T_f(i, j) \left(1 + \alpha \cdot \text{VOC}(i, j, t)^\beta \right)
$$

where:

- $T_f(i, j)$: Free-flow travel time,
- $\alpha$ and $\beta$: Parameters for congestion effects.

### Optimization Objective

Minimizing the difference between observed and predicted measurements, the objective function is:

$$
\min_{X} \quad \| Y - H \cdot X \|^2 + \lambda \| X - X_h \|^2
$$

where:

- $Y$: Measurement tensor,
- $H$: Mapping operator,
- $\lambda$: Regularization parameter.

### Traffic Flow Balance

For each node $n$, the balance condition is:

$$
\sum_{j} Q(j, n, t) = \sum_{k} Q(n, k, t)
$$




# Acknowledgements

The authors would like to thank the anonymous reviewers for their helpful suggestions to improve this paper.

# References


