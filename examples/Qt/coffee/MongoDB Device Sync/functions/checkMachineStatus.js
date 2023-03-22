exports = async function(changeEvent) {
  
    // Access the latest version of the changed document
    // (with Full Document enabled for Insert, Update, and Replace operations):
    const fullDocument = changeEvent.fullDocument;

    const db = context.services.get("mongodb-atlas").db("Coffee");
    // Access a mongodb service:
    const machineCollection = db.collection("CoffeeMachine");
    const drinksCollection = db.collection("DrinkTemplate");

    const drinks = await drinksCollection.find().toArray();
    
    var drinkSelection = []
   
    for (const idx in drinks) {
      // can it make 5 more drinks of this type?
      if ((fullDocument.espressoQty / drinks[idx].espressoQty) > 5 && 
          (fullDocument.chocolateQty / drinks[idx].chocolateQty) > 5 &&
          (fullDocument.milkQty / drinks[idx].milkQty) > 5) {
        drinkSelection.push(drinks[idx]._id);
      }
    }
    
    if (fullDocument.espressoQty < 20 || fullDocument.milkQty < 20 || fullDocument.chocolateQty < 20) {
      machineCollection.updateOne({"_id":changeEvent.documentKey._id}, { $set: {"state": 1, "availableDrinks": drinkSelection}});
    } else {
      machineCollection.updateOne({"_id":changeEvent.documentKey._id}, { $set: {"state": 0, "availableDrinks": drinkSelection}});
    }
};