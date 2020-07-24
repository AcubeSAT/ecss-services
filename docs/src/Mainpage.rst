ASAT CubeSat ECSS Services Library
==================================
|GitlabIssues|
   **Doc Note** This documentation is JUST a baseline example. Actual
   Information in this version WILL be wrong or nonsensical sometimes,
   in order to fill space or provide guidelines

Description
-----------

ECSS Services is a Packet and Services Library for the AcubeSat Project

This Library addresses the need to standardize the way the CubeSat
Interactions function between space and ground. It also standarizes and
defines the services and subservices that will be used by the diferent
Cubesat Systems. Using this library you will be able to enhance your
system with needed capabilities. As such, you should treat the services
in this library as a "menu" from which you can pick the ones that fit
your needs The two main components of the library are:

-  `Telecommand packets (TC)`_ and `Telemetry packets ( TM)`_ for remote
   monitoring and control of the ACubesat subsystems and payloads.
-  `Services`_ that satisfy all the fundamental operational requirements
   for spacecraft monitoring and control during spacecraft integration,
   testing and flight operations.

To start using ECSS Services Library, see the `User Guide`_ and `Getting
Started`_ documents.

   **Doc Note** If the provided links have user control. CubeSat Members will
   be alble to see the ecss standar but public user won't.

\*This standard is `provided in detail by ESA`_. The ECSS Services
library is the utilization of this standar in C++.

Status
------

======= =================================================
Version Status
======= =================================================
0.1.x   |Build Status|\ |GitHub issues|\ |PyPI status|
1.1.x   |Build Status|\ |GitHub issues|\ |GitHub release|
======= =================================================

License
-------

The content of this project itself is licensed under the |GitHub
license|, and the underlying source code used to format and display that
content is licensed under the |GitHub license| .

.. |GitHub license| image:: https://img.shields.io/badge/license-MIT-blue.svg
   :target: https://github.com/facebook/react/blob/master/LICENSE
.. |GitlabIssues| image:: https://helit.org/ecss-docs/docs/html/badge_is.svg
    :target: https://gitlab.com/acubesat/obc/ecss-services/issues/
    :alt: coverage report

.. _Telecommand packets (TC): #linkneeded
.. _Telemetry packets ( TM): #linkneeded
.. _Services: #linkneeded
.. _User Guide: #linkneeded
.. _Getting Started: #linkneeded
.. _provided in detail by ESA: #restrictedLink
.. _`https://github.com/facebook/react/blob/mas`: https://github.com/facebook/react/blob/mas

.. |Subreddit subscribers| image:: https://img.shields.io/reddit/subreddit-subscribers/cubeSat?style=social
.. |Twitter Follow| image:: https://img.shields.io/twitter/follow/dfsteassda?style=social
.. |Build Status| image:: https://travis-ci.org/joemccann/dillinger.svg?branch=master
   :target: https://travis-ci.org/joemccann/dillinger
.. |GitHub issues| image:: https://img.shields.io/github/issues/Naereen/StrapDown.js.svg
   :target: https://GitHub.com/Naereen/StrapDown.js/issues/
.. |PyPI status| image:: https://img.shields.io/pypi/status/ansicolortags.svg
   :target: https://pypi.python.org/pypi/ansicolortags/
.. |GitHub release| image:: https://img.shields.io/github/release/Naereen/StrapDown.js.svg
   :target: https://GitHub.com/Naereen/StrapDown.js/releases/