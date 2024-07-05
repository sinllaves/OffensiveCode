#used to store data in key value pairs and cant have duplicate keys

dict1 = {"a":1, "b":2, "c":3}
print(dict1)

print(len(dict1))

#cant use index like tuple, py has to find key

print(dict1["c"])
print(dict1.get("a"))

# see all keys
print(dict1.keys())

#see all values
print(dict1.values())

#see items

print(dict1.items())


#update

dict1["A"] = 1
print(dict1)

dict1.update({"a": 1})

# Delete

dict1.pop("a")
print(dict1)

del dict1["c"]


# Nested dictionary

dict1['c'] = {"a":1, "b":33}
print(dict1)

#keep a dict on standby // empty dictionary

dict2 = {}
print(dict2)

