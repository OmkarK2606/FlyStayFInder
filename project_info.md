# dsa_mini_project
# travel itinerary project for dsa using graphs and trees in c

> [!IMPORTANT]
> Please go through this text **every time** before beginning to code. If there are any changes youre making to this file, **send a message** in the WhatsApp group, and then make the change. Please check the checkboxes **only once you have finished coding the section, ran the code and made sure there are no errors**.

## what we have (data):
### 1. [flight itinerary csv file](./flights.csv), containing:
- from location
- to location
- price of flight
- duration of flight
- distance between [from] and [to]
- agency (is this required?)

### 2. [hotel list csv file](./hotels.csv), containing:
- travel code (is this required?)
- userCode (is this required?)
- hotel name (for now, each hotel name is a 6-letter randomly generated name)
- place ([to] location)
- price per day

## what we need to implement

- [ ] Reading data from csv files and storing them in an array / linked list
- [ ] Requesting data from the user, specifically,
  - Name (is this required?)
  - Source location
  - Destination
  - Number of days of travel
  - Ask user:
    - if they want to add any stops
    - for flight, do they want less time or less cost
    - for hotel, do they want most popular (based on ratings) or less cost
- [ ] Sort flight data according to user's preference and return
- [ ]  Sort hotel data according to user's preference and return
- [ ]   print an itinerary based on all the data collected

## steps for finding shortest duration / cost for flights

- read data from csv file [flights.csv](./flights.csv)
- store that data into an array (?) of structures
  - structure format:
    - source 
    - destination
    - price
    - duration
- graph structure:
  lets say that we want to go from A [source] to Z [destination].
  - from the array of structures, find all records that have source A.
  - lets say that there are 3 such records
    - A to B
    - A to C
    - A to D
  - next for each of B, C and D, do the same thing. Taking B, C, D as the sources, find the destinations for each.
  - do this for all the records in the array.
  - with this information, we will be able to construct a graph. (im still researching on how to do that)
  - after forming the graph, we can traverse the graph (there is an algorithm to do that) and we can find all the possible routes from A to Z.
  - store all possible routes.
  - sort the routes to find shortest duration / least price (based on user input).


> [!IMPORTANT]
> function to get data from csv **mishitha**
- input: ?
- output: array of structures

> [!IMPORTANT]
> function to sort all routes from [source] to [destination] **meera**
- input: array of structures (?)
- output: single structure (shortest duration / least cost)

> [!IMPORTANT]
> function to create graph **meghana**
- input: array of structures received from mishitha
- output: array of structures (?) containing all possible routes
