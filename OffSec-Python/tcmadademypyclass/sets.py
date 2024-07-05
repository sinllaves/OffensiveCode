# sets allow you to store multiple values in one variable
#cant be ordered and dont allow multiple values
#can not use indexes with sets

set1 = {"a", "b", "c"}
print(set1)

#another way to write out a set
set3 = {"a", 0, True}
print(set3)

set4 = set(("b", 1, False))
print(set1)


#no duplicates
set2 = {"a", "a", "a"}
print(set2)



# Add to set
set1.add("d")
print(set1)

# update existing sets
set3.update(set4)
print(set3)

# update existing sets with different data types (List ex. below)
list1 = ["a", "b", "c"]
set4 = {4,5,6}
print(list1)
print(set4)

set4.update(list1)
print(set4)

# Union of sets
set5 = {4, 5, 6}
set6 = set4.union(set4)
print(set6)

# Remove value. will error if key doesnt exist
set4.remove(4)
print(set4)

# Will Remove and not error
set4.discard(4)
print(set4)

# Only use pop if you dont care about the order

print(set1)

set1.pop()
print(set1)


