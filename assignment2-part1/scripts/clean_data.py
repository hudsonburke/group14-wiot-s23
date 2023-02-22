import matplotlib.pyplot as plt
import numpy as np

# Read in data
advertisements = list[str]
with open("./data/packets.csv", 'r') as file:
    advertisements.append(file.readline())

# Remove duplicates (shouldn't be any but just in case)
dupe_count = 0
num_ads = len(advertisements)
for i in range(num_ads):
    for j in range(num_ads):
        if advertisements[i] == advertisements[j]:
            dupe_count += 1
            print("Found duplicate #"+str(dupe_count))
            del advertisements[i]
# OR
unique_ads = [*set(advertisements)] # supposedly removes duplicates? https://www.geeksforgeeks.org/python-ways-to-remove-duplicates-from-list/

# Count
num_elements_per_advertisement = {}
num_advertisements_per_type = {}

for ad in advertisements:
    # csv line structure: src_addr, adv_type, payload{[LEN, TYPE, DATA], ...}
    data = ad.split() # splits on whitespace
    num_elements = (len(data)-2)/3
    if num_elements in num_elements_per_advertisement.keys():
        num_elements_per_advertisement[num_elements] += 1
    else:
        num_elements_per_advertisement[num_elements] = 0
    
    types = [*set(ad[3::2])] # supposedly removes duplicates? https://www.geeksforgeeks.org/python-ways-to-remove-duplicates-from-list/
    for t in types:
        if t in num_advertisements_per_type.keys():
            num_advertisements_per_type[t] += 1
        else:
            num_advertisements_per_type[t] = 0

# Make histograms
ad_type_labels = {"0x01": "Flags", "0x09": "Complete Local Name"} # fill in once we know what we need

fig, (ax1, ax2) = plt.subplots(1,2, sharey=True)

# Num advertisements v num AD elements
ax1.bar(range(len(num_elements_per_advertisement)), num_elements_per_advertisement.values(), tick_label=num_elements_per_advertisement.keys())
ax1.set_title("Advertisements with certain number of AD elements")
ax1.set_xlabel('Number of AD elements') 
ax1.set_ylabel("Number of advertisements")     

# Num advertisements v AD Type
ax2.bar(range(len(num_advertisements_per_type)), num_advertisements_per_type.values(), tick_label=num_advertisements_per_type.keys())
ax2.set_title("Advertisements with certain AD type")
ax2.set_xlabel('AD Type Specifier') 

fig.show()