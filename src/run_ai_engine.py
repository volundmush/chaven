from bs4 import BeautifulSoup
import random
import pandas as pd
import numpy as np
import os
from datetime import datetime
import openai
import re
import time
import signal
from datetime import datetime
import logging
import concurrent.futures
import csv
import json

logging.basicConfig(filename='../log/ai.log', level=logging.INFO)

class TimeoutError(Exception):
    pass

class timeout:
    def __init__(self, seconds=1, error_message='Timeout'):
        self.seconds = seconds
        self.error_message = error_message
    def handle_timeout(self, signum, frame):
        raise TimeoutError(self.error_message)
    def __enter__(self):
        signal.signal(signal.SIGALRM, self.handle_timeout)
        signal.alarm(self.seconds)
    def __exit__(self, type, value, traceback):
        signal.alarm(0)
        
    
def write_to_log(fprefix, fname, text):
    log_filename = f"/var/www/html/wiki/wiki.d/{fprefix}.{fname}"
    timestamp = int(datetime.now().timestamp())

    log_content = f"""version=pmwiki-2.2.92 ordered=1 urlencoded=1
agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/119.0.0.0 Safari/537.36
author=
charset=UTF-8
csum=
ctime={timestamp}
name={fprefix}.{fname}
rev=1
targets=
text={text}
time={timestamp}
"""
    # Check if the file exists and delete its content
    if os.path.exists(log_filename):
        with open(log_filename, 'w'):
            pass

    with open(log_filename, 'w') as log_file:
        log_file.write(log_content)


def to_html_color(text):

# replace `x with </span>
    text = text.replace('`x', '</span>')
    text = text.replace('`q', '</span>')

# replace `#(hexcode) with <span style="color:#(hexcode)">
    text = re.sub('`#([A-Fa-f0-9]{6})', r'<span style="color:#\1">', text)

# add a </span> before each new <span> if there isn't already an </span> in between
    text = re.sub('(<span style="color:#[A-Fa-f0-9]{6}">)(?![^<]*</span>)', '</span>\\1', text)

# remove leading </span>
    text = text.lstrip('</span>')


    return text


def cleanup_text(input_text):
    lines = input_text.split('\n')
    cleaned_lines = [line.replace('\r', '').strip() for line in lines if line.strip()]
    return '\n'.join(cleaned_lines)


def fetch_msg_history(filename, name_one, name_two):
    current_name_one = None
    current_name_two = None
    current_history = []
    messages = []

    with open(filename, 'r', encoding='utf-8', errors='ignore') as file:
        for line in file:
            if line.startswith('#TEXTHISTORY'):
                current_name_one = None
                current_name_two = None
                current_history = []
            elif line.startswith('NameOne'):
                current_name_one = re.search(r'NameOne (.+?)~', line).group(1)
            elif line.startswith('NameTwo'):
                current_name_two = re.search(r'NameTwo (.+?)~', line).group(1)
            elif line.startswith('History'):
                current_history.append(re.sub(r'^\^M', '', line.split(' ', 1)[1]))
            elif line.startswith('End'):
                if (current_name_one == name_one and current_name_two == name_two) or \
                   (current_name_one == name_two and current_name_two == name_one):
                    messages.extend(current_history)
                    break

    return current_name_one, current_name_two, cleanup_text('\n'.join(messages))





color_mapping = {
    '000': '#000000',
    '001': '#800000',
    '002': '#008000',
    '003': '#808000',
    '004': '#000080',
    '005': '#800080',
    '006': '#008080',
    '007': '#c0c0c0',
    '008': '#808080',
    '009': '#ff0000',
    '010': '#00ff00',
    '011': '#ffff00',
    '012': '#0000ff',
    '013': '#ff00ff',
    '014': '#00ffff',
    '015': '#ffffff',
    '016': '#000000',
    '017': '#00005f',
    '018': '#000087',
    '019': '#0000af',
    '020': '#0000d7',
    '021': '#0000ff',
    '022': '#005f00',
    '023': '#005f5f',
    '024': '#005f87',
    '025': '#005faf',
    '026': '#005fd7',
    '027': '#005fff',
    '028': '#008700',
    '029': '#00875f',
    '030': '#008787',
    '031': '#0087af',
    '032': '#0087d7',
    '033': '#0087ff',
    '034': '#00af00',
    '035': '#00af5f',
    '036': '#00af87',
    '037': '#00afaf',
    '038': '#00afd7',
    '039': '#00afff',
    '040': '#00d700',
    '041': '#00d75f',
    '042': '#00d787',
    '043': '#00d7af',
    '044': '#00d7d7',
    '045': '#00d7ff',
    '046': '#00ff00',
    '047': '#00ff5f',
    '048': '#00ff87',
    '049': '#00ffaf',
    '050': '#00ffd7',
    '051': '#00ffff',
    '052': '#5f0000',
    '053': '#5f005f',
    '054': '#5f0087',
    '055': '#5f00af',
    '056': '#5f00d7',
    '057': '#5f00ff',
    '058': '#5f5f00',
    '059': '#5f5f5f',
    '060': '#5f5f87',
    '061': '#5f5faf',
    '062': '#5f5fd7',
    '063': '#5f5fff',
    '064': '#5f8700',
    '065': '#5f875f',
    '066': '#5f8787',
    '067': '#5f87af',
    '068': '#5f87d7',
    '069': '#5f87ff',
    '070': '#5faf00',
    '071': '#5faf5f',
    '072': '#5faf87',
    '073': '#5fafaf',
    '074': '#5fafd7',
    '075': '#5fafff',
    '076': '#5fd700',
    '077': '#5fd75f',
    '078': '#5fd787',
    '079': '#5fd7af',
    '080': '#5fd7d7',
    '081': '#5fd7ff',
    '082': '#5fff00',
    '083': '#5fff5f',
    '084': '#5fff87',
    '085': '#5fffaf',
    '086': '#5fffd7',
    '087': '#5fffff',
    '088': '#870000',
    '089': '#87005f',
    '090': '#870087',
    '091': '#8700af',
    '092': '#8700d7',
    '093': '#8700ff',
    '094': '#875f00',
    '095': '#875f5f',
    '096': '#875f87',
    '097': '#875faf',
    '098': '#875fd7',
    '099': '#875fff',
    '100': '#878700',
    '101': '#87875f',
    '102': '#878787',
    '103': '#8787af',
    '104': '#8787d7',
    '105': '#8787ff',
    '106': '#87af00',
    '107': '#87af5f',
    '108': '#87af87',
    '109': '#87afaf',
    '110': '#87afd7',
    '111': '#87afff',
    '112': '#87d700',
    '113': '#87d75f',
    '114': '#87d787',
    '115': '#87d7af',
    '116': '#87d7d7',
    '117': '#87d7ff',
    '118': '#87ff00',
    '119': '#87ff5f',
    '120': '#87ff87',
    '121': '#87ffaf',
    '122': '#87ffd7',
    '123': '#87ffff',
    '124': '#af0000',
    '125': '#af005f',
    '126': '#af0087',
    '127': '#af00af',
    '128': '#af00d7',
    '129': '#af00ff',
    '130': '#af5f00',
    '131': '#af5f5f',
    '132': '#af5f87',
    '133': '#af5faf',
    '134': '#af5fd7',
    '135': '#af5fff',
    '136': '#af8700',
    '137': '#af875f',
    '138': '#af8787',
    '139': '#af87af',
    '140': '#af87d7',
    '141': '#af87ff',
    '142': '#afaf00',
    '143': '#afaf5f',
    '144': '#afaf87',
    '145': '#afafaf',
    '146': '#afafd7',
    '147': '#afafff',
    '148': '#afd700',
    '149': '#afd75f',
    '150': '#afd787',
    '151': '#afd7af',
    '152': '#afd7d7',
    '153': '#afd7ff',
    '154': '#afff00',
    '155': '#afff5f',
    '156': '#afff87',
    '157': '#afffaf',
    '158': '#afffd7',
    '159': '#afffff',
    '160': '#d70000',
    '161': '#d7005f',
    '162': '#d70087',
    '163': '#d700af',
    '164': '#d700d7',
    '165': '#d700ff',
    '166': '#d75f00',
    '167': '#d75f5f',
    '168': '#d75f87',
    '169': '#d75faf',
    '170': '#d75fd7',
    '171': '#d75fff',
    '172': '#d78700',
    '173': '#d7875f',
    '174': '#d78787',
    '175': '#d787af',
    '176': '#d787d7',
    '177': '#d787ff',
    '178': '#d7af00',
    '179': '#d7af5f',
    '180': '#d7af87',
    '181': '#d7afaf',
    '182': '#d7afd7',
    '183': '#d7afff',
    '184': '#d7d700',
    '185': '#d7d75f',
    '186': '#d7d787',
    '187': '#d7d7af',
    '188': '#d7d7d7',
    '189': '#d7d7ff',
    '190': '#d7ff00',
    '191': '#d7ff5f',
    '192': '#d7ff87',
    '193': '#d7ffaf',
    '194': '#d7ffd7',
    '195': '#d7ffff',
    '196': '#ff0000',
    '197': '#ff005f',
    '198': '#ff0087',
    '199': '#ff00af',
    '200': '#ff00d7',
    '201': '#ff00ff',
    '202': '#ff5f00',
    '203': '#ff5f5f',
    '204': '#ff5f87',
    '205': '#ff5faf',
    '206': '#ff5fd7',
    '207': '#ff5fff',
    '208': '#ff8700',
    '209': '#ff875f',
    '210': '#ff8787',
    '211': '#ff87af',
    '212': '#ff87d7',
    '213': '#ff87ff',
    '214': '#ffaf00',
    '215': '#ffaf5f',
    '216': '#ffaf87',
    '217': '#ffafaf',
    '218': '#ffafd7',
    '219': '#ffafff',
    '220': '#ffd700',
    '221': '#ffd75f',
    '222': '#ffd787',
    '223': '#ffd7af',
    '224': '#ffd7d7',
    '225': '#ffd7ff',
    '226': '#ffff00',
    '227': '#ffff5f',
    '228': '#ffff87',
    '229': '#ffffaf',
    '230': '#ffffd7',
    '231': '#ffffff',
    '232': '#080808',
    '233': '#121212',
    '234': '#1c1c1c',
    '235': '#262626',
    '236': '#303030',
    '237': '#3a3a3a',
    '238': '#444444',
    '239': '#4e4e4e',
    '240': '#585858',
    '241': '#626262',
    '242': '#6c6c6c',
    '243': '#767676',
    '244': '#808080',
    '245': '#8a8a8a',
    '246': '#949494',
    '247': '#9e9e9e',
    '248': '#a8a8a8',
    '249': '#b2b2b2',
    '250': '#bcbcbc',
    '251': '#c6c6c6',
    '252': '#d0d0d0',
    '253': '#dadada',
    '254': '#e4e4e4',
    '255': '#eeeeee',
}

def replace_xterm_color(match):
    color_code = match.group(1)
    replace = color_mapping.get(color_code, match.group(0))
    return "`" + replace

def replace_xterm_colors_in_text(text):
    pattern = re.compile(r'`(\d{3})')
    return pattern.sub(replace_xterm_color, text)


def wikify_text(input_text):
    input_text = input_text.replace("(SR", " SR ")

    # Remove non-alphanumeric characters
    cleaned_text = re.sub(r'[^a-zA-Z0-9 ]+', '', input_text)

    # Capitalize letters after a space
    transformed_text = ''.join(word.capitalize() for word in cleaned_text.split())

    # Remove non-alphanumeric characters and spaces
    t_text = re.sub(r'[^a-zA-Z0-9]+', '', transformed_text)


    return t_text
     
def replace_color_codes(match):
    color_code = match.group(1)
    # Convert xterm color code to hex
    hex_color = "#{:02X}{:02X}{:02X}".format(
        int(color_code[0] * 2), int(color_code[1] * 2), int(color_code[2] * 2)
    )
    return f"`{hex_color}"

def remove_hex_color(match):
    return ''
        
def extract_hist_section(filename):
    hist_section = ""
    hist_started = False
    with open(filename, 'r', encoding='utf-8', errors='ignore') as file:
        for line in file:
            if hist_started:
                if line.strip().endswith('~'):
                    hist_section += line.strip()[:-1]  # Remove the '~' character
                    break
                hist_section += line
            elif line.startswith('Hist '):
                hist_section += line.lstrip('Hist ')
                hist_started = True
    # Removing non-text characters
    clean_hist_section = ''.join(filter(str.isprintable, hist_section))
    return clean_hist_section  

def add_message(inlist, inrole, incontent):
   this_dict = {"role": inrole, "content": nohtml(incontent)}
   inlist.append(this_dict)
   return inlist

def nohtml(html):
    if(html is None):
        html = ""
    if(html is None):
        html = ""
    html = html.replace("<br>", "\n")
    html = html.replace("</p>", "\n")
    soup = BeautifulSoup(html, "html.parser")
    return soup.get_text().strip()

def word_in_text(word, text):
    match = re.search(r'\b' + word + r'(?:s|\'s)?\b', text, re.IGNORECASE)

    return match is not None


def extract_details(text):
    header, main_body = text.strip().split('\n\n', 1)

    matches = re.findall(r'([^:]+):\s*(\d+)', header)

    first_name = matches[0][0].strip() if matches else None
    first_number = int(matches[0][1]) if matches else None
    second_name = matches[1][0].strip() if len(matches) > 1 else None
    second_number = int(matches[1][1]) if len(matches) > 1 else None

    main_body = main_body.replace('Explanation:', '').strip()

    remaining_text = "\n".join([line for line in main_body.splitlines() if line.strip()])

    return first_name, first_number, second_name, second_number, remaining_text


def get_text_history(filename, name1, name2):
    nameone = None
    nametwo = None
    message_history = ''
    recording = False
    with open(filename, 'r') as file:
        for line in file:
            if '#TEXTHISTORY' in line:
                # Reset variables for the new history block
                nameone = None
                nametwo = None
                message_history = ''
                recording = False
            elif 'NameOne' in line:
                current_name = line.split()[1].rstrip('~')
                if name1 == current_name:
                    nameone = current_name
                elif name2 == current_name:
                    nametwo = current_name
            elif 'NameTwo' in line:
                current_name = line.split()[1].rstrip('~')
                if name1 == current_name:
                    nameone = current_name
                elif name2 == current_name:
                    nametwo = current_name
            elif 'History' in line and nameone and nametwo:
                # Start recording the message history
                recording = True
            elif recording:
                if 'End' in line or '#END' in line:
                    # Stop recording on 'End' and return the result if both names were found
                    if nameone and nametwo:
                        return nameone, nametwo, cleanup_text(message_history.strip())
                else:
                    # Replace '^M' with a newline character, remove timestamps, and add to message history
                    line_without_timestamps = re.sub(r'\[\w{3}\s+\d{1,2}\s+\d{2}:\d{2}\]', '', line).strip()
                    message_history += line_without_timestamps.replace('^M', '\n').strip() + '\n'
    return None, None, None  # Return None if the conversation was not found


def letter_to_xterm(otext):
    #This is the color code for says, feel free to change it.
    otext = otext.replace("`o", "`039")

    otext = otext.replace("`r", "`001")

    otext = otext.replace("`g", "`002")

    otext = otext.replace("`y", "`003")

    otext = otext.replace("`b", "`004")

    otext = otext.replace("`m", "`005")

    otext = otext.replace("`c", "`006")

    otext = otext.replace("`w", "`007")

    otext = otext.replace("`d", "`008")
    otext = otext.replace("`D", "`008")

    otext = otext.replace("`R", "`009")

    otext = otext.replace("`G", "`010")

    otext = otext.replace("`Y", "`011")

    otext = otext.replace("`B", "`012")

    otext = otext.replace("`M", "`013")

    otext = otext.replace("`C", "`014")

    otext = otext.replace("`W", "`015")
    return otext

def update_news_content(input_text):
    with open('/var/www/html/wiki/wiki.d/Main.HomePage', 'r') as file:
        lines = file.readlines()

    for i, line in enumerate(lines):
        if line.startswith('text='):
            content = re.findall(r'id="news_one">(.*?)%0a%3c/div>|id="news_two">(.*?)%0a%3c/div>|id="news_three">(.*?)%0a%3c/div>|id="news_four">(.*?)%0a%3c/div>|id="news_five">(.*?)%0a%3c/div>', line)
            content = [item for sublist in content for item in sublist if item]
            new_line = line.replace('id="news_one">'+content[0]+'%0a%3c/div>', 'id="news_one">'+input_text+'%0a%3c/div>').replace('id="news_two">'+content[1]+'%0a%3c/div>', 'id="news_two">'+content[0]+'%0a%3c/div>').replace('id="news_three">'+content[2]+'%0a%3c/div>', 'id="news_three">'+content[1]+'%0a%3c/div>').replace('id="news_four">'+content[3]+'%0a%3c/div>', 'id="news_four">'+content[2]+'%0a%3c/div>').replace('id="news_five">'+content[4]+'%0a%3c/div>', 'id="news_five">'+content[3]+'%0a%3c/div>')
            lines[i] = new_line
            break

    with open('/var/www/html/wiki/wiki.d/Main.HomePage', 'w') as file:
        file.writelines(lines)
        
def clean_aioutput(response_content):
    if(":" in response_content):
        response_content = response_content.split(":", 1)[1].strip()
    pattern = r'\([^)]*\)\s*'  # Matches anything inside parentheses including parentheses themselves
    cleaned_text = re.sub(pattern, '', response_content)
    return cleaned_text.strip()

def append_line_to_file(file_path, line):
    file_path = os.path.abspath(file_path)
    with open(file_path, 'a') as file:
        file.write(line + '\n')

def dstring():
    current_date = datetime.now().date()
    month_names = ["", "January", "February", "March", "April", "May", "June",
               "July", "August", "September", "October", "November", "December"]

    day = current_date.day
    month = month_names[current_date.month]
    year = current_date.year

    date_string = f"The {day} of {month} {year}"
    return date_string
        
def antag_faction_desc(antag_faction):
    if(antag_faction.lower() == "the destined host"):
        return "The Destined Host is a faction that believes in the power and supremacy of demons, viewing them as the world's only hope against impending doom. They have made pacts with these otherworldly entities, gaining power and influence in exchange for their loyalty and service. They work tirelessly to strengthen the demons' hold on the world, conducting dark rituals and seeking out ancient artifacts that could help to bring more of their benefactors into this realm. Despite their seemingly malevolent alliances, they see themselves as the world's protectors, believing that only by embracing the power of demons can the world be saved."
    if(antag_faction.lower() == "the golden shadow"):
        return "The Golden Shadow is a mercenary group that has chosen to profit from the world's impending destruction rather than try to prevent it. They are opportunists, working with whoever offers the most gain, be it dark forces or desperate individuals. Their ideology is one of ruthless pragmatism; they believe that in a world destined for destruction, the only sensible course of action is to amass as much wealth and power as possible while they can. They are skilled fighters and negotiators, using their talents to exploit the fear and chaos that the approaching apocalypse has caused."
    if(antag_faction.lower() == "the black flame"):
        return "The Black Flame is a cult that worships the eldritch horrors of the void, seeing them not as bringing about a necessary end. They believe that the world's destruction is not only inevitable, but desirable, a cleansing fire that will pave the way for whatever comes next. Their members are drawn from all walks of life, united by a shared fascination with the unknown and a desire to prepare the world for its impending end. They conduct rituals designed to hasten the arrival of the eldritch horrors, and work to spread their beliefs, hoping to convert others to their cause. Despite their dark and seemingly nihilistic ideology, they see themselves as the world's saviors, guiding it towards its ultimate destiny."
    if(antag_faction.lower() == "the sapphire martyrs"):
        return "The Sapphire Martyrs believe that nothing can be done to prevent the destruction of Earth but if the other worlds are still connected to it when its end comes, they too will perish. As such they are trying to achieve enough domination over the world to destroy it before its destined demise so as to sever those connections and save the other worlds. Their ranks are filled with individuals who have lost hope in salvation and are ready to embrace oblivion, believing in the purity of their cause. Their ideology is steeped in a profound sense of martyrdom, and they view their mission as a tragic but necessary act of love. Despite the horror of their objective, they carry themselves with a chilling serenity, seeing beauty in the sacrifice they are prepared to make."

    logging.info("No name match: " + antag_faction)
    return ""
        
        
def social_rate(name_one, name_two):
    
    nameone, nametwo, message_history = get_text_history("../data/texthistories.txt", name_one, name_two)
    ai_model = "gpt-4-1106-preview"
    intemp = 0.1
    out_msg = []
    ai_text = "Given the chat history below, rate " + nameone + " and " + nametwo + " with a score out of 100 on how interesting and engaging they are in this conversation. Give more points for being witty, interesting, funny, creating new conversational topics, asking questions, driving the conversation and making the conversation more interesting and less bland. Do not give points just for being nice. Return your results in the format\n" + nameone + ": (score)\n" + nametwo + ": (score)\n\nChat History:\n" + message_history

    add_message(out_msg, "user", ai_text)
    response = openai.ChatCompletion.create(
            model=ai_model,
            messages=out_msg,
            temperature=intemp,
            n=1,
    )
    logging.info(response)
    response_content = response.choices[0].message.content
    onone, sone, ontwo, stwo, exp = extract_details(response_content)

    outstring = "4|||" + nameone + "|||" + str(sone) + "|||" + nametwo + "|||" + str(stwo) + "|||" + exp
    append_line_to_file("../data/ai_out.csv", outstring)
    logging.info(outstring)
        
def describe_doom(char_name, days):
    ai_model = "gpt-4"
    #ai_model="gpt-3.5-turbo",
    intemp = 0.8
    max_size = 800
    out_msg = []
    fname = "../player/" + char_name
    hist_text = extract_hist_section(fname)
    otext = char_name + " is a character in a fictional horror setting. The setting is a small town in Massachusetts called Haven. In this setting supernatural forces such as vampires, werewolves and demons exist in secret. It is a horror setting, where power equals corruption. The character's history up to this point is:\n" + hist_text + "\nToday's date is " + dstring() + ". The character has been prophesied to die in " + str(days) + " days. Write the prophecy that foretells the character's death. It should be three of four sentences and include some details about how they are going to die. Do not include the exact date of their death. Start your response with 'Prophecy:'"
    add_message(out_msg, "user", otext)
    
    
    logging.info(out_msg)
    response = openai.ChatCompletion.create(
        model=ai_model,
        messages=out_msg,
        max_tokens=max_size,
        temperature=intemp,
        n=1,
    )
    logging.info(response)
    response_role = response.choices[0].message.role
    response_content = response.choices[0].message.content
    response_content = clean_aioutput(response_content)
    logging.info(response_content)
    outstring = "3|||" + char_name + "|||" + str(days) + "|||" + response_content
    logging.info(outstring)
    append_line_to_file("../data/ai_out.csv", outstring)
    
    
def describe_operation(antag_faction, area):
    ai_model = "gpt-4"
    #ai_model="gpt-3.5-turbo",
    intemp = 1.0
    max_size = 800
    out_msg = []
    plist = pd.read_csv('../data/pops.csv')
    prows = plist.sample(n=2, replace=False).sample(frac=1)
    first_process = prows.iloc[0]['process']
    first_description = cleanup_text(prows.iloc[0]['description'])
    second_process = prows.iloc[1]['process']
    second_description = cleanup_text(prows.iloc[1]['description'])
    
    authors = ["Jim Butcher", "Cassandra Clare", "Charlaine Harris", "Neil Gaiman", "Ben Aaronovitch", "Kate Daniels"]

    add_message(out_msg, "system", "You are " + random.choice(authors))

    add_message(out_msg, "user", "In an online roleplaying game in a supernatural setting different groups enage in battles in a psychic dream space called the nightmare which is an echo of the real world. For example groups may battle in an operation in a psychic mirror of a bank against the guards to steal money from a vault. If successful then in the real world the bank will be fated to lose the money and those that were succesfull will be fated to gain it. Here are two examples:\nprocess: " + first_process + "\ndescription: " + first_description + "\nprocess: " + second_process + "\ndescription: " + second_description + "\nA group called " + antag_faction + " is conducting an operation in " + area + ". This group is described as: " + antag_faction_desc(antag_faction) + "\nProvide parameters for the describe_operation function. Do not include details of the group in the description of the operation, readers will be already familiar with that information.")
    functions = [
        {
            "name": "describe_operation",
            "description": "Set the fields for an operation being run by a particular NPC faction.",
            "parameters": {
                "type": "object",
                "properties": {
                    "terrain": {
			            "type": "string",
			            "enum": ["forest", "field", "desert", "town", "city", "mountains", "warehouse", "caves", "village", "tundra", "lake"],
                        "description": "The type of terrain the battle will take place in."
		    },
 		    "area_name": {
			"type": "string",
			"description": "A short description of the area, e.g. 'inside westend bank' or 'a field outside San Fransisco'"
		    },
		    "process": {
			"type": "string",
                        "description": "A few words to describe the process that characters will need to do to win the operation. e.g. 'hacking into the system', or 'freeing the prisoners', or 'breaking into the vault'."
                    },
                    "description": {
			"type": "string",
			"description": "A one paragraph description of this operation and what the group is trying to achieve."
                },
                },
                "required": ["terrain", "area_name", "process", "description"],
            },
        }
    ]
    response = openai.ChatCompletion.create(
        model=ai_model,
        temperature=intemp,
        messages=out_msg,
        functions=functions,
#        function_call="describe_operation",  # auto is default, but we'll be explicit
    )
    logging.info(response)
    response_message = response["choices"][0]["message"]
    logging.info(response_message)
    try:
        function_args = json.loads(response_message["function_call"]["arguments"])
    except:
         function_args = json.loads(response_message["content"])
    terrain=function_args.get("terrain")
    area_name=function_args.get("area_name")
    process=function_args.get("process")
    description=function_args.get("description")
    
    outstring = "2|||" + antag_faction + "|||" + terrain + "|||" + area_name + "|||" + process + "|||" + description
    append_line_to_file("../data/ai_out.csv", outstring)
    
def get_random_name(gender):
    # Check if the provided gender is valid
    if gender not in ['M', 'F']:
        raise ValueError("Invalid gender. Please provide 'M' or 'F.")

    # Read the CSV file
    with open('../data/namelist.csv', 'r') as file:
        reader = csv.DictReader(file)
        names = [row['name'] for row in reader if row['sex'] == gender]

    # Check if there are names for the specified gender
    if not names:
        return f"No names found for gender '{gender}'."

    # Return a random name
    random_name = random.choice(names)


    script_directory = os.path.dirname(os.path.abspath(__file__))

    # Check if the name exists in the 'player' directory
    player_directory = os.path.join(script_directory, '..', 'player')
    if os.path.exists(os.path.join(player_directory, f"{random_name}")):
        return get_random_name(gender)
    else:
        # If the name is unique, return it
        return random_name
     
    
def create_operative(gcode, cname, csurname, cdesc, fdesc, cintro):
 logging.info("Creating Operative for: " + cname)
 with timeout(seconds=60):
  try:
    ai_model = "gpt-4"
    #ai_model="gpt-3.5-turbo",
    intemp = 1.0
    max_size = 800
    out_msg = []
    if(gcode == 1):
        nname = get_random_name('F')
    else:
        nname = get_random_name('M')
    
    authors = ["Jim Butcher", "Cassandra Clare", "Charlaine Harris", "Neil Gaiman", "Ben Aaronovitch", "Kate Daniels"]

    add_message(out_msg, "system", "You are " + random.choice(authors))

    message = cname + " is a character in a modern paranormal story and an operative in a group which is described as: " + fdesc + "\nHere are the parameters for that character.\nName: " + nname + "\nSurname: " + csurname + "\nShort_Description: " + cintro + "\nDescription: " + cdesc + "\n\n" + nname + " is a new operative who is sometimes mistaken for " + cname + " at a distance or in the dark. Provide parameters for the describe_operative function for " + nname + ". Do not mention that they are sometimes mistaken for " + cname + " and provide a unqiue surname as well as short_description and description which are meaningfully different to " + cname + "'s, just somewhat similar. The new character will not have dyed hair. Be creative and try to make the new operative consistent with the vibe of the group they are representing. Only include physical details in the description, not anything about their personality or group affiliation."

    functions = [
        {
            "name": "describe_operative",
            "description": "Set the fields for a new NPC operative.",
            "parameters": {
                "type": "object",
                "properties": {
                    "surname": {
                        "type": "string",
                        "description": "The surname of the character."
                    },
                    "short_description": {
                        "type": "string",
                        "description": "A short single sentence to express what the character looks like."
                    },
                    "description": {
                        "type": "string",
                        "description": "A one paragraph description of this character, should not include their name."
                    },
                    "hair_color": {
                        "type": "string",
                        "enum": ["black", "brown", "blond", "auburn", "chestnut", "red", "gray"],
                        "description": "The color of the character's hair."
                    },
                    "eye_color": {
                        "type": "string",
                        "enum": ["amber", "blue", "brown", "gray", "green", "hazel", "cerulean"],
                        "description": "The color of the character's eyes."
                    },
                    "height_feet": {
                        "type": "integer",
                        "minimum": 4,
                        "maximum": 7,
                        "description": "The height of the character in feet."
                    },
                    "height_inches": {
                        "type": "integer",
                        "minimum": 0,
                        "maximum": 11,
                        "description": "The height of the character in inches."
                    },
                    "skin_description": {
                        "type": "string",
                        "description": "A description of the character's skintone. Will be shown as they have (text) e.g. 'pale, freckled' or 'tanned' or 'olive skin'"
                    },
                },
                "required": ["surname", "short_description", "description", "hair_color", "eye_color", "height_feet", "height_inches", "skin_description"],
            },

        }
    ]
    add_message(out_msg, "user", message)
    response = openai.ChatCompletion.create(
        model=ai_model,
        temperature=intemp,
        messages=out_msg,
        functions=functions,
#        function_call="describe_operation",  # auto is default, but we'll be explicit
    )
    logging.info(response)
    response_message = response["choices"][0]["message"]
    logging.info(response_message)
    try:
        function_args = json.loads(response_message["function_call"]["arguments"])
    except:
         function_args = json.loads(response_message["content"])
    nsurname=function_args.get("surname")
    nintro=function_args.get("short_description")
    ndesc=function_args.get("description")
    hair_color=function_args.get("hair_color")
    eye_color=function_args.get("eye_color")
    height_feet=function_args.get("height_feet")
    height_inches=function_args.get("height_inches")
    skin_description=function_args.get("skin_description")
    
    outstring = "6|||" + cname + "|||" + nname + "|||" + nsurname + "|||" + nintro + "|||" + ndesc + "|||" + hair_color + "|||" + eye_color + "|||" + str(height_feet) + "|||" + str(height_inches) + "|||" + skin_description
    append_line_to_file("../data/ai_out.csv", outstring)
    logging.info(outstring)
  except:
    logging.info("Error creating operative for: " + cname)
    
    
    
def create_encounter(base_id):
  logging.info("Creating Encounter for: " + str(base_id))
  with timeout(seconds=60):

    elist = pd.read_csv('../data/elist.csv')

    ai_model = "gpt-4"
    
    intemp = 0.7
    max_size = 800
    out_msg = []
    
    authors = ["Jim Butcher", "Cassandra Clare", "Charlaine Harris", "Neil Gaiman", "Ben Aaronovitch", "Kate Daniels"]

    add_message(out_msg, "system", "You are " + random.choice(authors))

    erows = elist.query('ID != @base_id').sample(n=5, replace=False).sample(frac=1)

    itext = "Propose an encounter for a Dungeon Master to run for an individual character or small group of characters in a modern paranormal setting. The characters are located in Haven, a small town in Massachusetts where supernatural forces such as vampires, werewolves and demons exist in secret. It is a horror setting, where power equals corruption. Encounters should be small, self-contained stories, taking no longer than an hour or two to resolve. Start your response with 'Encounter:' \n"
    add_message(out_msg, "user", itext)
    rtext = "Encounter: " + erows.loc[erows.index[0], 'Text']
    add_message(out_msg, "assistant", rtext)
    for index, row in erows.iloc[1:].iterrows():
        nitext = "Propose another one."
        add_message(out_msg, "user", nitext)
        nrtext = "Encounter: " + row['Text']
        add_message(out_msg, "assistant", nrtext)
    #This could be commented out and replaced with the below.
    nitext = "Propose another one."
    add_message(out_msg, "user", nitext)
    nrtext = "Encounter: " + elist.loc[elist['ID'] == base_id, 'Text'].values[0]
    add_message(out_msg, "assistant", nrtext)
    add_message(out_msg, "user", nitext)

#    Similar circumstance matching doesn't seem to work well, maybe try again with gpt4 or pull the circumstance stuff from the code.
#    litext = "Propose another one, this time one that would work in a similar situation and enviroment to the following encounter, although the actual story should not be too similar: " + elist.loc[elist['ID'] == base_id, 'Text'].values[0]
#    add_message(out_msg, "user", litext)

    logging.info(out_msg)
    response = openai.ChatCompletion.create(
        model=ai_model,
        messages=out_msg,
        max_tokens=max_size,
        temperature=intemp,
        n=1,
    )
    logging.info(response)
    response_role = response.choices[0].message.role
    response_content = response.choices[0].message.content
    response_content = clean_aioutput(response_content)
    logging.info(response_content)
    outstring = "1|||" + str(base_id) + "|||" + response_content
    append_line_to_file("../data/ai_out.csv", outstring)
    
def make_html_text(otext):
    otext = letter_to_xterm(otext)
    otext = replace_xterm_colors_in_text(otext)
    
    hpattern = r"`#([0-9A-Fa-f]{6})"
    otext = otext.replace('\r', '</span>')
    otext = to_html_color(otext)
    otext = otext.replace("\n", "<br>")
    return otext

def website_news(nnews, snews):
     hnnews = make_html_text(nnews)
     hsnews = make_html_text(snews)
     
     html_text = "<b>Town News</b><br>" + hnnews + "<br><b>Supernatural News</b><br>" + hsnews + "<br><hr>"
     update_news_content(html_text)
     
     

def write_to_rplog(type, subtype, title_text, otext):

    wtitle = wikify_text(title_text)

    lines = otext.split('\n')

    filtered_lines = [line for line in lines if "See society report" not in line]

    # Join the remaining lines
    otext = '\n'.join(filtered_lines)

    otext = letter_to_xterm(otext)
    otext = replace_xterm_colors_in_text(otext)
    
    hpattern = r"`#([0-9A-Fa-f]{6})"
    strip_text = re.sub(hpattern, remove_hex_color, otext)
    otext = otext.replace('\r', '</span>')
    xpattern = r"`(\d{3})"
    strip_text = re.sub(xpattern, replace_color_codes, strip_text)
    fpattern = r'`.'
    strip_text = re.sub(fpattern, '', strip_text)
    strip_text = strip_text.replace('\r', '')
    
    ai_model = "gpt-4-1106-preview"
    intemp = 1.0
    out_msg = []

    if(subtype == 4):
        ai_text = "Write a one page summary of the roleplay below as a story emphasizing important moments and character actions. Don't include details on the setup, readers will already be familiar with this. Be sure to include how the story concludes. Start your response with 'Summary:'\n\n" + strip_text
    elif(type == 1):
        ai_text = "Write a two paragraph summary of the roleplay below as a story emphasizing important moments and character actions, feel free to embellish and be creative in order to make a more compelling story. Don't include details on the setup, readers will already be familiar with this. Be sure to include how the story concludes. Start your response with 'Summary:'\n\n" + strip_text
    else:
        ai_text = "Write a two paragraph summary of the roleplay below as a story emphasizing important moments and character actions. Don't include details on the setup, readers will already be familiar with this. Be sure to include how the story concludes. Start your response with 'Summary:'\n\n" + strip_text

    add_message(out_msg, "user", ai_text)
    summ_content = ""
    logging.info(out_msg)
    if(type == 1 or subtype != 2):
        response = openai.ChatCompletion.create(
            model=ai_model,
            messages=out_msg,
            temperature=intemp,
            n=1,
        )
        logging.info(response)
        response_role = response.choices[0].message.role
        response_content = response.choices[0].message.content
        response_content = clean_aioutput(response_content)
        logging.info(response_content)
    else:
        response_content = ""
        

    if(type == 1 or subtype != 2):
        n_out = []
        nai_text = "Write a one sentence summary of this text. Start your response with 'Summary:'\n\n" + response_content
        add_message(n_out, "user", nai_text)
        response = openai.ChatCompletion.create(
            model=ai_model,
            messages=n_out,
            temperature=0.5,
            n=1,
        )
        logging.info(response)
        response_role = response.choices[0].message.role
        summ_content = response.choices[0].message.content
        summ_content = clean_aioutput(summ_content)
    html_text = to_html_color(otext)

    if(type == 1):
        html_text = response_content + "<hr>" + html_text
    else:
        html_text = response_content + "<hr><span " + html_text
    html_text = html_text.replace("\n", "<br>")


    pre_text = "<style>.lfloat {float:none ! important; margin-right: 30px !important; display: inline-block ! important;}.container{margin: 10px 10px 10px 10px;padding: 10px 10px 10px 10px;background-color: black;border-style: solid;border-width: thin;color: #f0f0f0;color; ##f0f0f0;}h1, h2, h3, h4, h5, h6, .sidehead {color: white;}#newcharacterform{height: 70px;padding: 10px 10px 10px 10px;}.frame{margin: 0px 0px 20px 0px;color:black;} .container form {padding: 5px 10px 10px 10px; width: 678px; height: 100px;}.frame {overflow: clip !important; height: 188px !important; !important; width: 172px !important;}</style><div class=\"container\">"


    html_text = pre_text + html_text + "</div>"
    if(type == 1):
        ltype = "Operationlogs"
    else:
        if(subtype == 1):
            ltype = "Encounterlogs"
        elif(subtype == 2):
            ltype = "Patrollogs"
        elif(subtype == 3):
            ltype = "Encounterlogs"
        elif(subtype == 4):
            ltype = "Plotlogs"
        else:
            ltype = "Playerlogs"

    wtitle = wtitle + datetime.now().strftime('%y%m%d')
    write_to_log(ltype, wtitle, html_text)

    newscontent = response_content.replace("\n", "<br>")
    newscontent = newscontent + "<br>[[" + ltype + "/" + wtitle + "|-" + title_text + "]]<br><hr>"

    if(subtype != 2):
        update_news_content(newscontent)
        if(summ_content != ""):
            url = "http://havenrpg.net/wiki/pmwiki.php/" + ltype + "/" + wtitle
            output = summ_content + ": " + url
            outstring = "5|||" + output + "|||" + url + "|||" + title_text
            append_line_to_file("../data/ai_sum_out.csv", outstring)
            append_line_to_file("../data/ai_sum_out.tmp", outstring)


openai.organization = ""
openai.api_key = ""

starttime = time.time()




while True:
  logging.info(datetime.fromtimestamp(time.time()))
  ai_input = pd.read_csv('../data/ai_in.csv')
  ai_input_unique = ai_input.drop_duplicates()
  ai_input_unique.to_csv('../data/ai_in.csv', index=False, header=True)
  ai_input = pd.read_csv('../data/ai_in.csv')
  ai_header = pd.read_csv('../data/ai_in.csv', nrows=0)
  ai_header.to_csv('../data/ai_in.csv', index=False, header=True)

  ai_count = ai_input.shape[0]
  
  if(ai_count > 0):
    for i in range(ai_count):
#      try:
        if(ai_input.iloc[i]['Type'] == 1):
            create_encounter(ai_input.iloc[i]['ID'])
        if(ai_input.iloc[i]['Type'] == 2):
            describe_operation(ai_input.iloc[i]['ValOne'], ai_input.iloc[i]['ValTwo'])
        if(ai_input.iloc[i]['Type'] == 3):
            describe_doom(ai_input.iloc[i]['ValOne'], ai_input.iloc[i]['ValTwo'])
        if(ai_input.iloc[i]['Type'] == 4):
            social_rate(ai_input.iloc[i]['ValOne'], ai_input.iloc[i]['ValTwo'])       
        if(ai_input.iloc[i]['Type'] == 5):
            website_news(ai_input.iloc[i]['ValOne'], ai_input.iloc[i]['ValTwo'])      
        if(ai_input.iloc[i]['Type'] == 6):
            create_operative(ai_input.iloc[i]['ID'], ai_input.iloc[i]['ValOne'], ai_input.iloc[i]['ValTwo'], ai_input.iloc[i]['ValThree'], ai_input.iloc[i]['ValFour'], ai_input.iloc[i]['ValFive'])
#      except:
#          logging.info("Error processing row: " + str(i))
    
#  exit()


  sum_input = pd.read_csv('../data/ai_sum_in.csv', quoting=csv.QUOTE_MINIMAL, quotechar='~', engine='python')
  sum_count = sum_input.shape[0]
  
  # Create an empty DataFrame with the same columns
  empty_df = pd.DataFrame(columns=sum_input.columns)

# Save the empty DataFrame to the same CSV file, effectively overwriting it
  empty_df.to_csv('../data/ai_sum_in.csv', index=False)
  
  if(sum_count > 0):
      for i in range(sum_count):
        try:
            write_to_rplog(sum_input.iloc[i]['type'], sum_input.iloc[i]['subtype'], sum_input.iloc[i]['title'], sum_input.iloc[i]['text'])
        except:
          logging.info("Error processing rplog row: " + str(i))


  if(10.0 - ((time.time() - starttime) % 10.0) > 0):
    time.sleep(10.0 - ((time.time() - starttime) % 10.0))
