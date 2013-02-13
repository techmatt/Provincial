
Provincial is an artifical intelligence for the card game Dominion.

For more information, see graphics.stanford.edu/~mdfisher/DominionAI.html

The website contains all the details on reading the strategy visualizations, but the
general rule is that an AI purchases the left-most card it can afford.

Some important things to remember:

 * Provincial must train on a set of kingdom cards before playing it effectively.
   For simplicity, the application comes with a large number of pre-trained kingdoms
   which are located in Provincial/data/kingdoms_g32_c1, and when you start a new
   game an AI is loaded at random from this folder. However, if you specify a custom
   kingdom or uncheck the "Use precomputed kingdoms" box, the AI will make random
   decisions until you train a new set of strategies for the new kingdom.
   
 * The application uses a large set of custom cards that can be found in the
   Provincial/data/custom folder. To disable these cards, edit the
   Provincial/data/parameters.txt file and set useCustomCards to false.

 * Provnicial.exe is the main application. It will launch TestinGroundsInternal.exe
   when a new tournament is requested. If you want to continuously run random
   kingdoms, launch TestingGroundsSpawner.exe and click the Go button.
