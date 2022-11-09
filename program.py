import pygame
import sys

from pygame.locals import *

screen = pygame.display.set_mode((400 , 300))

while True:

    screen.fill([0,0,0])

    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit(0)

    pygame.display.flip()

