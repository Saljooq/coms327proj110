Name: Saljooq Altaf
Net ID: saltaf


Assignment 1.10


This is a continuation of the work done in 1.01-1.09

New features added to this version:

1/
The menu of equipment ('e') and inventory ('w') could only be accessed from the
main menu. So, if you were in the inventory menu and had to go to equipment menu,
 you needed to press 'b' or ESC and then press 'e'. However, now you directly
 press 'e' to go to equipment, and if you want to go back to inventory, press 'w'.

 2/
 There was no way to tell the hitpoints of PC previously, or how many or which enemies are attacking.
 However, now we get a report of all the attacks between turns underneath the game grid.
 The reports include the name of the of the enemy that attacked, damaged it incurred,
 and the amount of hp left in PC. All damage reports are in RED.

 3/
 There is a new option visibility added alongside a circle of visibility rather than the
 square. Pressing 'v' allows the users to increase or decrease
 radius of visibility by pressing UP or DOWN. To go back to playing game, we press 'b' or ESC.
 Additionally, player can see the visibility with + signs -> to adjust the radius.


 4/
 Entagled - This is a new feature that stops the enemies from running away from a fight with PC.
 The method unentagle() is used to unentagle all PC, allowing the enemy to run away if PC doesn't wish
 to continue to fight. Entangled enemy WILL attack PC back unless it has been unentangled.

 5/
 Easier to identify and kill Boss to finish level -
 Since winning the game requires killing the BOSS and if BOSS is unique, the enemy will never be
 spawned again if we move to another level. This makes winning the game impossible. Also, unless we
 read the Monsters doc, we wouldn't know who the boss is. In this new update, moving to another level
 isn't possible if Boss has been detected and isn't defeated. Also, menu of monsters 'm' now shows
 '<---BOSS' of a boss is present in the level, to identify it.
