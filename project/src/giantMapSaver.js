export default class GiantMapSaver {
    constructor(giantMap)
    {
        this.MAX_SIZE = 100000;
        this.deconstructed = []
        this.giantMap = giantMap;
        
    }
    deconstruct(){
        this.deconstructed = [];
        let currentCount = 0;
        let indexOfArray = 0;
        this.deconstructed.push([]);
        for(let ent of this.giantMap.entries())
        {
            if (currentCount < this.MAX_SIZE)
            {
                this.deconstructed[indexOfArray].push(ent);
                currentCount += 1;
            } else
            {
                indexOfArray += 1;
                currentCount = 0;
                this.deconstructed.push([]);
            }

        }
        return this.deconstructed;
    }
}
