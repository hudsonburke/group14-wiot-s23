import matplotlib.pyplot as plt

# Read in data
advertisements = []
with open("./data/packets_final.csv", 'r') as file:
    advertisements = [line.rstrip() for line in file]

# Remove Duplicates
advertisements = [*set(advertisements)] # supposedly removes duplicates? https://www.geeksforgeeks.org/python-ways-to-remove-duplicates-from-list/

# Count
num_advertisements_with_num_elements = {} # Key = Num Elements, Value = Num advertisements w/ that num elements
num_advertisements_per_type = {} # Key = Type, Value = Num advertisements w/ that type

for ad in advertisements:
    # csv line structure: src_addr, adv_type, payload{[LEN, TYPE, DATA], ...}
    data = ad.split(',') # splits on commas
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

# Sort
num_advertisements_per_type = dict(sorted(num_advertisements_per_type.items()))
num_advertisements_with_num_elements = dict(sorted(num_advertisements_with_num_elements.items()))
print(num_advertisements_per_type)
print(num_advertisements_with_num_elements)

# Label
ad_type_labels = {
    "0xff": "Manufacturer Specific Data", 
    "0x01": "Flags",
    "0x0a": "Tx Power Level",
    "0x16": "Service Data - 16-bit UUID",
    "0x03": "Complete List of 16-bit Service Class UUIDs",
    "0x09": "Complete Local Name",
    "0x02": "Incomplete List of 16-bit Service Class UUIDs",
    "0x19": "Appearance",
    "0x05": "Complete List of 32-bit Service Class UUIDs",
    "0x06": "Incomplete List of 128-bit Service Class UUIDs",
    "0x07": "Complete List of 128-bit Service Class UUIDs"
    } # from https://btprodspecificationrefs.blob.core.windows.net/assigned-numbers/Assigned%20Number%20Types/Assigned%20Numbers.pdf
for key in list(num_advertisements_per_type.keys()):
    num_advertisements_per_type[str(key) + ": " + ad_type_labels[key]] = num_advertisements_per_type[key]
    del num_advertisements_per_type[key]

# Make histograms

# Num advertisements v num AD elements
plt.figure(1)
# plt.bar(range(len(num_advertisements_with_num_elements)), num_advertisements_with_num_elements.values(), tick_label=list(num_advertisements_with_num_elements.keys()), align="edge", width=1.0)
plt.bar(num_advertisements_with_num_elements.keys(), num_advertisements_with_num_elements.values(), align="edge", width=1.0)
plt.title("Advertisements with certain number of AD elements")
plt.xlabel('Number of AD elements') 
plt.ylabel("Number of advertisements")     

# Num advertisements v AD Type
plt.figure(2)
plt.bar(num_advertisements_per_type.keys(), num_advertisements_per_type.values(), align="edge", width=1.0)
plt.xticks(rotation=45, ha='right')
plt.title("Advertisements with certain AD type")
plt.xlabel('AD Type Specifier') 
plt.ylabel("Number of advertisements") 

plt.tight_layout()
plt.show()
