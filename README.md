# EE Notation Parser

Converts the format internally used by Warframe's engine — the Evolution Engine — into JSON.

## CLI Tool Example Usage

\> cat example.ee
```
CharacterName=/Lotus/Language/1999/MessengerAoiName
LocFile=/Lotus/Language/1999Texting/AoiRomance.loc
CompatibleGifts={
    {
        Item=/Lotus/Types/Gameplay/1999Wf/Gifts/JerryCanLarge
        GainedChemistry=5
        Cost=0
        GiftedQuantity=0
    },
    {
        Item=/Lotus/Types/Gameplay/1999Wf/Gifts/Headphones
        GainedChemistry=20
        Cost=0
        GiftedQuantity=0
    },
    {
        Item=/Lotus/Types/Gameplay/1999Wf/Gifts/CompactDisc
        GainedChemistry=20
        Cost=0
        GiftedQuantity=0
    },
    {
        Item=/Lotus/Types/Gameplay/1999Wf/Gifts/Scooter
        GainedChemistry=20
        Cost=0
        GiftedQuantity=0
    },
    {
        Item=/Lotus/Types/Gameplay/1999Wf/Gifts/PackagedStereoAmp
        GainedChemistry=20
        Cost=0
        GiftedQuantity=0
    },
    {
        Item=/Lotus/Types/Gameplay/1999Wf/Gifts/Sign
        GainedChemistry=5
        Cost=0
        GiftedQuantity=0
    }
}
```

\> ee2json example.ee example.json

\> cat example.json
```json
{
    "CharacterName": "/Lotus/Language/1999/MessengerAoiName",
    "LocFile": "/Lotus/Language/1999Texting/AoiRomance.loc",
    "CompatibleGifts": [
        {
            "Item": "/Lotus/Types/Gameplay/1999Wf/Gifts/JerryCanLarge",
            "GainedChemistry": 5,
            "Cost": 0,
            "GiftedQuantity": 0
        },
        {
            "Item": "/Lotus/Types/Gameplay/1999Wf/Gifts/Headphones",
            "GainedChemistry": 20,
            "Cost": 0,
            "GiftedQuantity": 0
        },
        {
            "Item": "/Lotus/Types/Gameplay/1999Wf/Gifts/CompactDisc",
            "GainedChemistry": 20,
            "Cost": 0,
            "GiftedQuantity": 0
        },
        {
            "Item": "/Lotus/Types/Gameplay/1999Wf/Gifts/Scooter",
            "GainedChemistry": 20,
            "Cost": 0,
            "GiftedQuantity": 0
        },
        {
            "Item": "/Lotus/Types/Gameplay/1999Wf/Gifts/PackagedStereoAmp",
            "GainedChemistry": 20,
            "Cost": 0,
            "GiftedQuantity": 0
        },
        {
            "Item": "/Lotus/Types/Gameplay/1999Wf/Gifts/Sign",
            "GainedChemistry": 5,
            "Cost": 0,
            "GiftedQuantity": 0
        }
    ]
}
```
