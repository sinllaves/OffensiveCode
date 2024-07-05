#can store multiple items in a single variable
# cant change whats in parathensis once it has been defined


tuple_items = "item1", "item2", "item3"
print(tuple_items)

tuple_numbers = (1,2,3)
print(tuple_numbers)

tuple_repeat = ('combine!',) * 4
print(tuple_repeat)

mixed_tuple = ("a", 1, ("A", 2))
print(mixed_tuple)

tuple_combined = tuple_items + tuple_numbers
print(tuple_combined)

item1, item2, item3 = tuple_items
print(item1)
print(item2)
print(item3)

#look for in tuple
print("item2"in tuple_items)
print("item3"in tuple_items)
print("item4"in tuple_items)

#index of item in tuple - Py starts with 0 then 1, 2,
print(tuple_items.index("item2"))
#so above is 1

#print items index place
tuple_items = ("item1", "item2", "item3")
print(tuple_items[0])
print(tuple_items[1])
print(tuple_items[2])

#how many tuple items

print(len(tuple_items))

# what tuple item is in what place from the end not the start
# so -1 would is the last place, -2 would be second from last
print(tuple_items[-1])
print(tuple_items[-2])

# slice, we can choose what indexs we want
print(tuple_items[0:2])
print(tuple_items[:2])

#Slice, start from back, remember up to and exluding last item, -3 included -1 excluded
print(tuple_items[-3:-1])

#Slice, Can also slice strings
string1 = "I am a string!"

#print string index
print(string1[0:4])

#minus the !
print(string1[:-1])




# 





