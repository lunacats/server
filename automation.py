

class node:
    x = 0.0
    y = 0.0


def main( ):
    # the bot may try to map out the entire arena
    # if that's the case then it could benefit from
    # knowing the dimensions of the arena.

    # all measurements in meters, and all measurements gathered
    # from information available on the 2018 competition
    # if it's not for the 2018 competition, verify the
    # dimensions are the same, then update these values
    # and this comment!


    startzonex = 1.89
    startzoney = 1.5
    obszonex = 3.78     #Obstacle zone x
    obszoney = 2.94      #Obstacle zone y
    minezonex = 3.78
    minezoney = 2.94

    arenax = obszonex
    arenay = startzoney + obszoney + minezoney

    # locate the mining area
        # if the mining area or the collection bin is located
        # the bot will be able to logically assume where the
        # other is

        # alternatively, it could just mathematically
        # find the mining area, like something like this
        # this will get center of mining area, assuming
        # larger y is farther from the collection bin
    miningnode = node()
    miningnode.x = arenax/2.0
    miningnode.y = arenay - minezoney
        # if we go with this method, we'll still need to figure
        # out where the bot and where it's facing somehow
        # will need to use sensors and their libraries

    # turn to face mining area
        # one possible way would be to move to the center point
        # of the start zone, and align with the collector
        # but face the opposite way

    # move towards the mining area until a significant obstacle
    # has been encountered. will need to avoid obstacles

    # how to define and move to destination node
    n = node()
    n.x = 0.0
    n.y = 0.0
    moveto(n)





# n is the destination node
def moveto( n ):
    # first we plan the route
    # a few different algorithms exist for this purpose
    # one of them is Bug 2. it's not very efficient but could be easier
    # to implement
    # A* is probably the best

    # suppose A*'s been implemented and returns a list of nodes
    node2 = node()
    n.x = 0.0
    n.y = 0.0
    movetohelper(node2)

# recursive solution to moveto?
def movetohelper( n ):
    # might not actually use this
    exit()