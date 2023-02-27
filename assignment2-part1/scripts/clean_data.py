import matplotlib.pyplot as plt
import numpy as np

# Read in data
advertisements = []
with open("./data/packets3.csv", 'r') as file:
    advertisements = [line.rstrip() for line in file]

# Remove duplicates (shouldn't be any but just in case)
# dupe_count = 0
# num_ads = len(advertisements)
# for i in range(num_ads):
#     for j in range(num_ads):
#         if advertisements[i] == advertisements[j]:
#             dupe_count += 1
#             print("Found duplicate #"+str(dupe_count))
#             del advertisements[i]
# OR
advertisements = [*set(advertisements)] # supposedly removes duplicates? https://www.geeksforgeeks.org/python-ways-to-remove-duplicates-from-list/

# Count
num_advertisements_with_num_elements = {} # Key = Num Elements, Value = Num advertisements w/ that num elements
num_advertisements_per_type = {} # Key = Type, Value = Num advertisements w/ that type

for ad in advertisements:
    # csv line structure: src_addr, adv_type, payload{[LEN, TYPE, DATA], ...}
    data = ad.split(',') # splits on commas
    # num_elements = data[-1] # counted by scanner
    num_elements = (len(data)-2)/3 
    if num_elements in num_advertisements_with_num_elements.keys():
        num_advertisements_with_num_elements[num_elements] += 1
    else:
        num_advertisements_with_num_elements[num_elements] = 1
    types = [*set(data[3::3])] 
    for t in types:
        if t in num_advertisements_per_type.keys():
            num_advertisements_per_type[t] += 1
        else:
            num_advertisements_per_type[t] = 1

print(num_advertisements_per_type)
print(num_advertisements_with_num_elements)
# Make histograms
ad_type_labels = {
    "0x01": "Flags", 
    "0x09": "Complete Local Name",
    } # fill in once we know what we need

fig, (ax1, ax2) = plt.subplots(1,2)

# Num advertisements v num AD elements
# ax1.hist(num_advertisements_with_num_elements)
# ax1.stairs(num_elements_per_advertisement.values(), num_elements_per_advertisement.keys(), fill=True)
ax1.bar(range(len(num_advertisements_with_num_elements)), num_advertisements_with_num_elements.values(), tick_label=list(num_advertisements_with_num_elements.keys()), align="edge", width=1.0)
ax1.set_title("Advertisements with certain number of AD elements")
ax1.set_xlabel('Number of AD elements') 
ax1.set_ylabel("Number of advertisements")     

# Num advertisements v AD Type
# ax2.hist(num_advertisements_per_type)
# ax2.stairs(num_advertisements_per_type.values(), num_advertisements_per_type.keys(), fill=True)
ax2.bar(range(len(num_advertisements_per_type)), num_advertisements_per_type.values(), tick_label=list(num_advertisements_per_type.keys()), align="edge", width=1.0)
ax2.set_title("Advertisements with certain AD type")
ax2.set_xlabel('AD Type Specifier') 
ax2.set_ylabel("Number of advertisements")     

plt.show()
