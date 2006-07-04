# contents: Needle registry for Ned.
#
# Copyright © 2005 Nikolai Weibull <nikolai@bitwi.se>



# ¶ \section{Dependency Injection}
#
# \DefineTerm{Dependency injection} is a software pattern that decouple’s the
# creation and initialization of objects from the rest of their
# responsibilities.  The idea is to lift the relationship between objects in a
# system a level higher in abstraction.  We will begin by introducing the
# problem that dependency injection tries to solve, then we introduce an
# intermediate solution known as the \Term{service locator}, and finally we
# explain how dependency injection works and how it solves the suggested
# problem.
#
# \subsection{Issues relating to the creation and initialization of objects.}
#
# Creating and initializing objects in an object||oriented system can be a
# complex task.  There are often many interconnections between different
# subsystems and between objects within these subsystems.  It can thus be a
# complicated procedure to create the right objects and make sure that the
# appropriate connections are made between them.  The straightforward solution
# is to simply create a dependency graph and traverse it, creating objects as
# one goes along and passing on the appropriate objects to their dependents.
#
# The main problem with this solution is that such a system has very tight
# coupling, both within subsystems and between them.  It can thus be very hard
# to make modifications to connected objects and also to add new dependencies
# to the system, as a small change may ripple through the whole system.
# Another problem is that it can be hard to plug in mock objects into the
# system, as the concrete names of the classes will necessarily have to be used
# in this setup.  One must also make sure that every object is created in the
# right order, as they need to be passed “live” to their dependents.
#
# \subsection{The intermediate solution~--~service locators.}
#
# Dependency injection is actually an extension to another software pattern
# known as the \DefineTerm{service locator}.  The service locator pattern
# removes the explicit references to class names in the objects of our system
# and reverses the logic of initialization, instead allowing objects to request
# the objects they need instead of being passed the ones that we think they
# want.
#
# The idea is to move the explicit references to a container that will act as
# the service locator.  Object in our system will then request the objects they
# require from the service locator, instead of creating them themselves or
# having them passed as arguments to initializers.  This means that classes can
# easily be replaced in the container when needed and the change will be kept
# local to the creation of the container itself.
#
# There are, however, some problems with this pattern:
#
# \startitemize
#   \item It requires each object in the system to accept the locator as an
#     argument during initialization.  This can be a problem if parts of the
#     system were created without knowledge of the service locator and the
#     locator can’t easily be retrofitted onto them, e.g., if those parts
#     aren’t available for modification.
#  
#   \item It requires each object to know the names of the services it needs.
#     Thus, we’re sort of back where we started, in that we have only moved the
#     coupling from between the objects to between the objects and the service
#     locator.
#  
#   \item When the dependency graph is deep, the locator will have to be passed
#     along just as far as any other object needed during initialization would
#     have been.
#  
#   \item It doesn’t alleviate the problem of getting the order||of||creation
#     right.
# \stopitemize
#
# \subsection{Taking on more responsibilities with dependency injection.}
#
# The dependency injection pattern tries to solve the problems outlined at the
# end of the previous subsection by taking on the responsibility of creating
# objects as well.  Using dependency injection, all the information about the
# creation and coupling of objects is kept within the container, now known as a
# \DefineTerm{registry}.
#
# The immediate effect of this is that objects can once again be written
# knowing nothing of the service locator itself.  All they to do is make sure
# that their dependencies can be passed as parameters to their initializers or
# have them accessible through property accessors.  This solves problems 1 and
# 3 suggested in the previous subsection.
#
# Using the dependency injection pattern, we also don’t need to worry about the
# effects of giving names to services, as their use will be constricted to the
# registry itself, thus minimizing any effects of renaming or similar.  Thus,
# problem 2 has also been solved.
#
# Problem 4 is a bit more interesting, as it isn’t immediately obvious how
# dependency injection may solve it.  The thing is, as the registry is more or
# less a representation of the dependency graph and we have localized object
# (node) creation to it, all we need to do is defer creation of objects until
# their node is visited when traversing the graph.  Thus, objects aren’t
# created until they are needed.
#
# The dependency||injection registry can also be used to dictate object
# lifestyles.  Normally, objects in a registry are \DefineTerm{singletons},
# i.e., there exists only a sole instance of the object.  Not all objects will
# require such solitude.  Alternative lifestyles include:
#
# \term{Threaded} Objects that are singletons within each thread of execution.
# \term{Multiton} Objects that are singletons for each set of initialization
#   parameters.
# \term{Prototype} Objects that are created anew for each request.
#
# Having such diversity in the lifestyle of imaginary objects in a computer is
# as beneficial as it is in the real world.
#
# \subsection{Encapsulating subsystems using service libraries.}
#
# To make dependency injection somewhat simpler to use, we introduce a notion
# that encapsulates a set of services and makes them available for
# registration in a registry.  The idea is that subsystems can define a set of
# services that it wants to make available to the system as a whole|<|or other
# systems for that matter|>|and that set may then be included by a registry so
# that those services become available to other services in that registry.
# 
# We will see more of these so called \DefineTerm{service libraries} as we
# begin discussing our actual implementation.


# ¶ \section{Implementing Dependency Injection}
#
# The dependency injection pattern has been implemented for Ruby in a library
# called \DefineTerm{needle} \cite[Needle].  We will be using this library, but
# we create our own extension to the registry class so that we can use it as a
# singleton itself.

require 'needle'
require 'needle/extras'
require 'singleton'



module Ned
  class Registry < Needle::Registry
    include Singleton

    def initialize(opts = {})
      super
    end
  end
end


# ¶ In the current version of needle, the \Ruby{require_library} method is
# missing from the \Ruby{DefinitionContext} class.  This isn’t a big problem,
# but having it in that class as well makes our life easier in defining and
# using service libraries.

module Needle
  class DefinitionContext
    def require_library(name)
      Kernel.require name
      LIBRARIES[name].call(self) if LIBRARIES[name]
    end
  end
end
