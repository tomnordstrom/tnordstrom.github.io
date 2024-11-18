
"""
To create an exe file:
cd .\Jobb\Knife_chart\ 
& "C:\Users\tnordstrom\AppData\Local\Packages\PythonSoftwareFoundation.Python.3.12_qbz5n2kfra8p0\LocalCache\local-packages\Python312\Scripts\pyinstaller.exe" --onefile --windowed .\Knife_guide.py
"""

import pygame

screen_width = 800
screen_height = 625
button_spacing = 25
button_colors = [(200, 200, 200), (100, 100, 255), (0, 255, 0)]
button_dimensions = [125, 50]
spare_parts = {
    "561": {
        "Old hexagonal": {
            "refurbished": "802902",
            "new": "does not exist",
            "complete upgrade kit refurbished": "811188 + 827829",
            "complete upgrade kit new": "809518 + 827829",
            "received_knife": "802885 or 802902 or 802670"
        },
        "New piston shaped": {
            "refurbished": "811188",
            "new": "809518",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "809518 or 811188 or 809517"
        }
    },
    "561W": {
        "Old hexagonal": "does not exist",
        "New piston shaped": {
            "refurbished": "825274",
            "new": "823447",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "823447 or 825274 or 813560"
        }
    },
    "562": {
        "Old hexagonal": {
            "refurbished": "802902",
            "new": "does not exist",
            "complete upgrade kit refurbished": "811188 + 827959",
            "complete upgrade kit new": "809518 + 827959",
            "received_knife": "802885 or 802902 or 802670"
        },
        "New piston shaped": {
            "refurbished": "811188",
            "new": "809518",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "809518 or 811188 or 809517"
        }
    },
    "C20": {
        "Old hexagonal": {
            "refurbished": "802902",
            "new": "does not exist",
            "complete upgrade kit refurbished": "811188 + 809519",
            "complete upgrade kit new": "809518 + 809519",
            "received_knife": "802885 or 802902 or 802670"
        },
        "New piston shaped": {
            "refurbished": "811188",
            "new": "809518",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "809518 or 811188 or 809517"
        }
    },
    "C22 1st knife": {
        "Old hexagonal": {
            "refurbished": "802902",
            "new": "does not exist",
            "complete upgrade kit refurbished": "811188 + 809519",
            "complete upgrade kit new": "809518 + 809519",
            "received_knife": "802885 or 802902 or 802670"
        },
        "New piston shaped": {
            "refurbished": "811188",
            "new": "809518",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "809518 or 811188 or 809517"
        }
    },
    "C22 2nd knife": {
        "Old hexagonal": {
            "refurbished": "808780",
            "new": "808852",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "808852 or 808780 or 808397"
        },
        "New piston shaped": "does not exist"
    },
    "C23 1st knife": {
        "Old hexagonal": "does not exist",
        "New piston shaped": {
            "refurbished": "811188",
            "new": "809518",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "809518 or 811188 or 809517"
        }
    },
    "C23 2nd knife": {
        "Old hexagonal": {
            "refurbished": "811651",
            "new": "811645",
            "complete upgrade kit refurbished": "does not exist",
            "complete upgrade kit new": "does not exist",
            "received_knife": "811645 or 811651 or 811440"
        },
        "New piston shaped": "does not exist"
    }
}

machine_names = list(spare_parts.keys())
knife_types = list(spare_parts[machine_names[0]].keys())
knife_to_send_back_options = ["refurbished", "new", "complete upgrade kit refurbished", "complete upgrade kit new"]

machine_object = []


def button(screen, color, x, y, width, height, text='', text_color=(0,0,0), text_size=30):
    pygame.draw.rect(screen, color, (x, y, width, height))
    font = pygame.font.Font(None, text_size)
    text = font.render(text, True, text_color)
    screen.blit(text, (x + (width/2 - text.get_width()/2), y + (height/2 - text.get_height()/2)))


class Machine:
    def __init__(self, name):
        self.name = name
        self.knife_type = ""
        self.knife_to_send_back_option = ""
        self.received_knife = ""


def create_machine(name):
    choice = [machine_names[name]]
    machine_object.clear()
    machine_object.append(Machine(choice[0]))


if __name__ == "__main__":
    
    pygame.init()
    screen = pygame.display.set_mode((screen_width, screen_height))
    pygame.display.set_caption("Knife guide")
    running = True

    while running:
        screen.fill((255, 255, 255))
        pygame.draw.rect(screen, (150, 150, 150), (0, 0, 338, 625))
        pygame.draw.rect(screen, (125, 125, 125), (338, 0, 262, 625))
        pygame.draw.rect(screen, (100, 100, 100), (588, 0, 212, 625))
        # Draw explanatory text
        font = pygame.font.Font(None, 24)
        text = font.render("Select a machine and knife type received", True, (0, 0, 0))
        text_rect = text.get_rect(left=5, top=5)
        screen.blit(text, text_rect)

        text_2 = font.render("Send back", True, (0, 0, 0))
        text_rect_2 = text_2.get_rect(right=screen_width - 300, top=5)
        screen.blit(text_2, text_rect_2)
        
        text_3 = font.render("Article number", True, (0, 0, 0))
        text_rect_3 = text_3.get_rect(right=screen_width - 50, top=5)
        screen.blit(text_3, text_rect_3)

        # Replace the single text_4 render with multiple renders
        text_4_line1 = font.render("Check if availablein stock", True, (0, 0, 0))
        text_4_line2 = font.render("(from top to bottom)", True, (0, 0, 0))
        
        # Position the text lines
        text_rect_4_line1 = text_4_line1.get_rect(centerx= 465, top=350)
        text_rect_4_line2 = text_4_line2.get_rect(centerx= 465, top=text_rect_4_line1.bottom + 5)  # 5 pixels gap between lines
        
        # Blit both lines to the screen
        screen.blit(text_4_line1, text_rect_4_line1)
        screen.blit(text_4_line2, text_rect_4_line2)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
    
            if event.type == pygame.MOUSEBUTTONDOWN:
                mouse_pos = pygame.mouse.get_pos()
                if 25 <= mouse_pos[0] <= 150 and 25 <= mouse_pos[1] <= 75:
                    create_machine(0)
                if 25 <= mouse_pos[0] <= 150 and 100 <= mouse_pos[1] <= 150:
                    create_machine(1)
                if 25 <= mouse_pos[0] <= 150 and 175 <= mouse_pos[1] <= 225:
                    create_machine(2)
                if 25 <= mouse_pos[0] <= 150 and 250 <= mouse_pos[1] <= 300:
                    create_machine(3)
                if 25 <= mouse_pos[0] <= 150 and 325 <= mouse_pos[1] <= 375:
                    create_machine(4)
                if 25 <= mouse_pos[0] <= 150 and 400 <= mouse_pos[1] <= 450:
                    create_machine(5)
                if 25 <= mouse_pos[0] <= 150 and 475 <= mouse_pos[1] <= 525:
                    create_machine(6)
                if 25 <= mouse_pos[0] <= 150 and 550 <= mouse_pos[1] <= 600:
                    create_machine(7)
                if machine_object and machine_object[0].name in machine_names:
                    if 175 <= mouse_pos[0] <= 325 and 25 <= mouse_pos[1] <= 75 and spare_parts[machine_object[0].name][knife_types[0]] != "does not exist":
                        machine_object[0].knife_type = knife_types[0]
                    if 175 <= mouse_pos[0] <= 325 and 100 <= mouse_pos[1] <= 150 and spare_parts[machine_object[0].name][knife_types[1]] != "does not exist":
                        machine_object[0].knife_type = knife_types[1]
                if machine_object and machine_object[0].name and machine_object[0].knife_type:
                    if 350 <= mouse_pos[0] <= 475+100 and 25 <= mouse_pos[1] <= 75 and spare_parts[machine_object[0].name][machine_object[0].knife_type][knife_to_send_back_options[0]] != "does not exist":
                        machine_object[0].knife_to_send_back_option = knife_to_send_back_options[0]
                    if 350 <= mouse_pos[0] <= 475+100 and 100 <= mouse_pos[1] <= 150 and spare_parts[machine_object[0].name][machine_object[0].knife_type][knife_to_send_back_options[1]] != "does not exist":
                        machine_object[0].knife_to_send_back_option = knife_to_send_back_options[1]
                    if 350 <= mouse_pos[0] <= 475+100 and 175 <= mouse_pos[1] <= 225 and spare_parts[machine_object[0].name][machine_object[0].knife_type][knife_to_send_back_options[2]] != "does not exist":
                        machine_object[0].knife_to_send_back_option = knife_to_send_back_options[2]
                    if 350 <= mouse_pos[0] <= 475+100 and 250 <= mouse_pos[1] <= 300 and spare_parts[machine_object[0].name][machine_object[0].knife_type][knife_to_send_back_options[3]] != "does not exist":
                        machine_object[0].knife_to_send_back_option = knife_to_send_back_options[3]
                else:
                    continue

                print(machine_object[0].__dict__)
    
        for i in range(len(machine_names)):
            if machine_object and machine_object[0].name == machine_names[i]:
                button(screen, button_colors[1], 25, 25 + i * 75, button_dimensions[0], button_dimensions[1], machine_names[i], (0, 0, 0), 20)
            else:
                button(screen, button_colors[0], 25, 25 + i * 75, button_dimensions[0], button_dimensions[1], machine_names[i], (0, 0, 0), 20)
                
    
        if machine_object and machine_object[0].name in machine_names:
            for j in range(len(knife_types)):
                if spare_parts[machine_object[0].name][knife_types[j]] == "does not exist":
                    continue
                if machine_object[0].knife_type == knife_types[j]:
                    button(screen, button_colors[1], 175, 25 + j * 75, button_dimensions[0]+25, button_dimensions[1], knife_types[j], (0, 0, 0), 20)
                    machine_object[0].received_knife = spare_parts[machine_object[0].name][knife_types[j]]["received_knife"]
                    # Draw explanatory text
                    font = pygame.font.Font(None, 16)
                    text_5 = font.render(machine_object[0].received_knife, True, (0, 0, 0))
                    text_rect_5 = text_5.get_rect(left=175, top=58 + j * 75)
                    screen.blit(text_5, text_rect_5) 
                else:
                    button(screen, button_colors[0], 175, 25 + j * 75, button_dimensions[0]+25, button_dimensions[1], knife_types[j], (0, 0, 0), 20)
                    # Draw explanatory text
                    machine_object[0].received_knife = spare_parts[machine_object[0].name][knife_types[j]]["received_knife"]
                    font = pygame.font.Font(None, 16)
                    text_6 = font.render(machine_object[0].received_knife, True, (0, 0, 0))
                    text_rect_6 = text_6.get_rect(left=175, top=58 + j * 75)
                    screen.blit(text_6, text_rect_6) 
    
        if machine_object and machine_object[0].name in machine_names and machine_object[0].knife_type in knife_types:
            for k in range(len(knife_to_send_back_options)):
                if spare_parts[machine_object[0].name][machine_object[0].knife_type][knife_to_send_back_options[k]] == "does not exist":
                    continue
                if machine_object[0].knife_to_send_back_option == knife_to_send_back_options[k]:
                    button(screen, button_colors[1], 350, 25 + k * 75, button_dimensions[0] + 100, button_dimensions[1], knife_to_send_back_options[k], (0, 0, 0), 20)
                else:
                    button(screen, button_colors[0], 350, 25 + k * 75, button_dimensions[0] + 100, button_dimensions[1], knife_to_send_back_options[k], (0, 0, 0), 20)
        
        if machine_object and machine_object[0].name in machine_names and machine_object[0].knife_type in knife_types and machine_object[0].knife_to_send_back_option in knife_to_send_back_options:
            button(screen, button_colors[2], 500 + 130, 25, button_dimensions[0], button_dimensions[1], spare_parts[machine_object[0].name][machine_object[0].knife_type][machine_object[0].knife_to_send_back_option], (0, 0, 0), 20)
        
        pygame.display.flip()
    
    pygame.quit()
