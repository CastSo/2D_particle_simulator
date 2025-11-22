# 2D Particle Simulator
![sand_sim](https://github.com/user-attachments/assets/f290f9fe-0131-4997-ada8-75bed643cdac)

The implementation uses Margolus Neighborhood, where a transition to a new state is determined by a 2X2 cell block.
The realistic toppling behavior was achieved by iterating over four grid, which forces the nonempty cell to alternate directions for each iteration.[^1]

# Work Cited
[^1]: Probabilistic Cellular Automata for Granular Media in Video Games, Jonathan Devlin, Micah D. Schuster, https://arxiv.org/pdf/2008.06341
