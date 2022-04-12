---
title: "Better way of knowing carrier provider for contacts in India"
date: 2022-04-12T23:22:41+05:30
tag: linux
---

This is a result of late night call from my dad asking me if I remember/know a mobile carrier of one of our contacts. There is an easy way to find this by opening any mobile recharge apps and put the details if suggests you the carrier details but that might proved to be wrong once you move closer to recharge, and so you have to go back repeat the step with multiple operators until you get success.

well this post is exactly that, only that it talks in terms to HTTP requests

I choose freecharge APIs as demo here, since I use to like the app when it was just started.

I plan to do some form of automation with the idea so that I can answer that question quickly and accurately next time :)

## 1. Fetch meta-data

fetch meta data, we are mostly interested in operatorMasterId from each item in the list

#### REQUEST

```
GET https://www.freecharge.in/api/catalog/nosession/sub-category/SHORT_CODE/MR
```

#### RESPONSE

```json
[
  {
    ...
    "name": "Airtel Prepaid",
    ...
    "operatorMasterId": 650,
    "customOperatorMasterId": 650,
    ...
  },
  {
    ...
    "name": "BSNL Prepaid",
    ...
    "operatorMasterId": 651,
    "customOperatorMasterId": 651,
    ...
  },
  {
    ...
    "name": "Jio Prepaid",
    ...
    "operatorMasterId": 652,
    "customOperatorMasterId": 652,
    ...
  },
  {
    ...
    "name": "MTNL Delhi Prepaid",
    ...
    "operatorMasterId": 648,
    "customOperatorMasterId": 648,
    ...
  },
  {
    ...
    "name": "MTNL Mumbai Prepaid",
    ...
    "operatorMasterId": 649,
    "customOperatorMasterId": 649,
    ...
  },
  {
    ...
    "name": "Vi Prepaid",
    ...
    "operatorMasterId": 653,
    "customOperatorMasterId": 653,
    ...
  }
]
```

## 2. get correct circleId and "probably" correct operatorId

#### REQUEST

```
POST https://www.freecharge.in/api/fulfilment/nosession/fetch/operatorMapping

{"serviceNumber":"7739877732","productCode":"MR"}
```

#### RESPONSE

```json
{
  "error": null,
  "data": {
    "circleId": 3,
    "operatorId": 650,
    "productType": "Mobile Recharge",
    "productCode": "MR"
  }
}
```

## 3. Final request to confirm operator

here we have to make multiple requests with each operatorId that we obtained in step 1, keeping the circleId same until we get success response thus knowing the correct operator.

#### REQUEST

```
POST https://www.freecharge.in/api/fulfilment/nosession/bill/fetch

{"authenticators":[{"parameterName":"Consumer Number","parameterValue":"7739877732"}],"operatorId":651,"category":"Mobile Recharge","circleId":3}
```

#### SUCCESS RESULT

```json
{
  "error": null,
  "data": {
    "billFetchId": null,
    "userDetails": [],
    "billDetails": [],
    "additionalDetails": []
  }
}
```

#### ERROR RESULT

```json
{
  "error": {
    "errorCode": "IN_ERR_2002",
    "errorMessage": "You've entered invalid details. Please select correct operator and circle for your number.",
    "metaData": "null"
  },
  "data": null
}
```
